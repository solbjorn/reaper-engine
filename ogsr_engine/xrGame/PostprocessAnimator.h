#ifndef __ppanimator_included__
#define __ppanimator_included__

#include "..\xr_3da\envelope.h"
#include "..\xr_3da\EffectorPP.h"
#include "..\xr_3da\cameramanager.h"

class CEffectorController;

#define POSTPROCESS_PARAMS_COUNT 11
#define POSTPROCESS_FILE_VERSION 0x0002

#define POSTPROCESS_FILE_EXTENSION ".ppe"

typedef enum _pp_params
{
    pp_unknown = -1,
    pp_base_color = 0,
    pp_add_color = 1,
    pp_gray_color = 2,
    pp_gray_value = 3,
    pp_blur = 4,
    pp_dual_h = 5,
    pp_dual_v = 6,
    pp_noise_i = 7,
    pp_noise_g = 8,
    pp_noise_f = 9,
    pp_cm_influence = 10,
    pp_last = 11,
    pp_force_dword = 0x7fffffff
} pp_params;

class XR_NOVTABLE CPostProcessParam : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPostProcessParam);

public:
    ~CPostProcessParam() override = 0;

    virtual void update(f32 dt) = 0;
    virtual void load(IReader& pReader) = 0;
    virtual void save(IWriter& pWriter) = 0;
    [[nodiscard]] virtual f32 get_length() = 0;
    [[nodiscard]] virtual size_t get_keys_count() = 0;
};

inline CPostProcessParam::~CPostProcessParam() = default;

class CPostProcessValue : public CPostProcessParam
{
    RTTI_DECLARE_TYPEINFO(CPostProcessValue, CPostProcessParam);

protected:
    CEnvelope m_Value;
    float* m_pfParam;

public:
    explicit CPostProcessValue(float* pfparam) : m_pfParam{pfparam} {}
    ~CPostProcessValue() override = default;

    void update(f32 dt) override { *m_pfParam = m_Value.Evaluate(dt); }
    void load(IReader& pReader) override;
    void save(IWriter& pWriter) override;

    [[nodiscard]] f32 get_length() override
    {
        float mn, mx;
        return m_Value.GetLength(&mn, &mx);
    }

    [[nodiscard]] size_t get_keys_count() override { return m_Value.keys.size(); }
};

class CPostProcessColor : public CPostProcessParam
{
    RTTI_DECLARE_TYPEINFO(CPostProcessColor, CPostProcessParam);

protected:
    float m_fBase{};
    CEnvelope m_Red;
    CEnvelope m_Green;
    CEnvelope m_Blue;
    SPPInfo::SColor* m_pColor;

public:
    explicit CPostProcessColor(SPPInfo::SColor* pcolor) : m_pColor{pcolor} {}
    ~CPostProcessColor() override = default;

    void update(f32 dt) override
    {
        m_pColor->r = m_Red.Evaluate(dt);
        m_pColor->g = m_Green.Evaluate(dt);
        m_pColor->b = m_Blue.Evaluate(dt);
    }

    void load(IReader& pReader) override;
    void save(IWriter& pWriter) override;

    [[nodiscard]] f32 get_length() override
    {
        float mn, mx, r = m_Red.GetLength(&mn, &mx), g = m_Green.GetLength(&mn, &mx), b = m_Blue.GetLength(&mn, &mx);
        mn = (r > g ? r : g);
        return mn > b ? mn : b;
    }

    [[nodiscard]] size_t get_keys_count() override { return m_Red.keys.size(); }
};

class CPostprocessAnimator : public CEffectorPP
{
    RTTI_DECLARE_TYPEINFO(CPostprocessAnimator, CEffectorPP);

protected:
    CPostProcessParam* m_Params[POSTPROCESS_PARAMS_COUNT];
    shared_str m_Name;
    SPPInfo m_EffectorParams;
    float m_factor;
    float m_dest_factor;
    bool m_bStop;
    float m_factor_speed;
    bool m_bCyclic;
    float m_start_time;
    float f_length;

    void Update(float tm);

public:
    CPostprocessAnimator();
    explicit CPostprocessAnimator(int id, bool cyclic);
    ~CPostprocessAnimator() override;

    void Clear();
    void Load(gsl::czstring name);
    [[nodiscard]] auto Name() const { return m_Name.c_str(); }
    void Stop(f32 speed) override;
    void SetDesiredFactor(float f, float sp);
    void SetCurrentFactor(float f);
    void SetCyclic(bool b) { m_bCyclic = b; }
    [[nodiscard]] f32 GetLength();
    [[nodiscard]] BOOL Valid() override;
    [[nodiscard]] BOOL Process(SPPInfo& PPInfo) override;
    void Create();
};

class CPostprocessAnimatorLerp : public CPostprocessAnimator
{
    RTTI_DECLARE_TYPEINFO(CPostprocessAnimatorLerp, CPostprocessAnimator);

protected:
    CallMe::Delegate<float()> m_get_factor_func;

public:
    ~CPostprocessAnimatorLerp() override = default;

    void SetFactorFunc(CallMe::Delegate<float()> f) { m_get_factor_func = f; }
    [[nodiscard]] BOOL Process(SPPInfo& PPInfo) override;
};

class CPostprocessAnimatorLerpConst : public CPostprocessAnimator
{
    RTTI_DECLARE_TYPEINFO(CPostprocessAnimatorLerpConst, CPostprocessAnimator);

protected:
    f32 m_power{1.0f};

public:
    CPostprocessAnimatorLerpConst() = default;
    ~CPostprocessAnimatorLerpConst() override = default;

    void SetPower(f32 val) { m_power = val; }
    [[nodiscard]] BOOL Process(SPPInfo& PPInfo) override;
};

class CPostprocessAnimatorControlled : public CPostprocessAnimatorLerp
{
    RTTI_DECLARE_TYPEINFO(CPostprocessAnimatorControlled, CPostprocessAnimatorLerp);

public:
    CEffectorController* m_controller;

    explicit CPostprocessAnimatorControlled(CEffectorController* c);
    ~CPostprocessAnimatorControlled() override;

    [[nodiscard]] BOOL Valid() override;
};

#endif /*__ppanimator_included__*/
