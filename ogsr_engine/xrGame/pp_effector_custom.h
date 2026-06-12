#pragma once

#include "../xr_3da/effectorPP.h"
#include "../xr_3da/cameramanager.h"

//////////////////////////////////////////////////////////////////////////

class CPPEffectorCustom : public CEffectorPP
{
    RTTI_DECLARE_TYPEINFO(CPPEffectorCustom, CEffectorPP);

public:
    typedef CEffectorPP inherited;

    explicit CPPEffectorCustom(const SPPInfo& ppi, bool one_instance = false, bool destroy_from_engine = true);
    ~CPPEffectorCustom() override = default;

    [[nodiscard]] EEffectorPPType get_type() { return m_type; }

protected:
    [[nodiscard]] BOOL Process(SPPInfo& pp) override;

    // update factor; if return FALSE - destroy
    [[nodiscard]] virtual BOOL update() { return TRUE; }

private:
    SPPInfo m_state;
    EEffectorPPType m_type;

protected:
    float m_factor;
};

//////////////////////////////////////////////////////////////////////////

template <class _Effector>
class CPPEffectorCustomController : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPPEffectorCustomController<_Effector>);

public:
    CPPEffectorCustomController() = default;
    ~CPPEffectorCustomController() override = default;

    inline virtual void load(gsl::czstring section);
    [[nodiscard]] inline virtual bool active() { return !!m_effector; }

protected:
    _Effector* m_effector{};
    SPPInfo m_state;
};

template <class _Effector>
void CPPEffectorCustomController<_Effector>::load(LPCSTR section)
{
    m_state.duality.h = pSettings->r_float(section, "duality_h");
    m_state.duality.v = pSettings->r_float(section, "duality_v");
    m_state.gray = pSettings->r_float(section, "gray");
    m_state.blur = pSettings->r_float(section, "blur");
    m_state.noise.intensity = pSettings->r_float(section, "noise_intensity");
    m_state.noise.grain = pSettings->r_float(section, "noise_grain");
    m_state.noise.fps = pSettings->r_float(section, "noise_fps");
    VERIFY(!fis_zero(m_state.noise.fps));

    auto res = scn::scan<f32, f32, f32>(std::string_view{pSettings->r_string(section, "color_base")}, "{},{},{}");
    R_ASSERT(res, res.error().msg());

    const auto [br, bg, bb] = res->values();
    m_state.color_base.r = br;
    m_state.color_base.g = bg;
    m_state.color_base.b = bb;

    res = scn::scan<f32, f32, f32>(std::string_view{pSettings->r_string(section, "color_gray")}, "{},{},{}");
    R_ASSERT(res, res.error().msg());

    const auto [gr, gg, gb] = res->values();
    m_state.color_gray.r = gr;
    m_state.color_gray.g = gg;
    m_state.color_gray.b = gb;

    res = scn::scan<f32, f32, f32>(std::string_view{pSettings->r_string(section, "color_add")}, "{},{},{}");
    R_ASSERT(res, res.error().msg());

    const auto [ar, ag, ab] = res->values();
    m_state.color_add.r = ar;
    m_state.color_add.g = ag;
    m_state.color_add.b = ab;
}

//////////////////////////////////////////////////////////////////////////

class CPPEffectorController;

class CPPEffectorControlled : public CPPEffectorCustom
{
    RTTI_DECLARE_TYPEINFO(CPPEffectorControlled, CPPEffectorCustom);

private:
    typedef CPPEffectorCustom inherited;

    CPPEffectorController* m_controller;

public:
    explicit CPPEffectorControlled(CPPEffectorController* controller, const SPPInfo& ppi, bool one_instance = false, bool destroy_from_engine = true);
    ~CPPEffectorControlled() override = default;

    [[nodiscard]] BOOL update() override;
    IC void set_factor(float value) { m_factor = value; }
};

//////////////////////////////////////////////////////////////////////////

class XR_NOVTABLE CPPEffectorController : public CPPEffectorCustomController<CPPEffectorControlled>
{
    RTTI_DECLARE_TYPEINFO(CPPEffectorController, CPPEffectorCustomController<CPPEffectorControlled>);

public:
    CPPEffectorController();
    ~CPPEffectorController() override;

    virtual void frame_update();

    [[nodiscard]] virtual bool check_completion() = 0;
    [[nodiscard]] virtual bool check_start_conditions() = 0;
    virtual void update_factor() = 0;

    // factory method
    [[nodiscard]] virtual CPPEffectorControlled* create_effector() = 0;

protected:
    void activate();
    void deactivate();
};
