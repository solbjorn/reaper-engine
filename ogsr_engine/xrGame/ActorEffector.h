#pragma once

#include "CameraEffector.h"

class CObjectAnimator;
class CEffectorController;
class CActor;

class CActorCameraManager : public CCameraManager
{
    RTTI_DECLARE_TYPEINFO(CActorCameraManager, CCameraManager);

public:
    SCamEffectorInfo m_cam_info_hud;

protected:
    using inherited = CCameraManager;

    virtual void UpdateCamEffectors();
    virtual bool ProcessCameraEffector(CEffectorCam* eff);

public:
    CActorCameraManager() : inherited(false) {}
    virtual ~CActorCameraManager() {}
    IC void hud_camera_Matrix(Fmatrix& M) { M.set(m_cam_info_hud.r, m_cam_info_hud.n, m_cam_info_hud.d, m_cam_info_hud.p); }
};

using GET_KOEFF_FUNC = CallMe::Delegate<float()>;

void AddEffector(CActor* A, int type, const shared_str& sect_name);
void AddEffector(CActor* A, int type, const shared_str& sect_name, float factor);
void AddEffector(CActor* A, int type, const shared_str& sect_name, GET_KOEFF_FUNC);
void AddEffector(CActor* A, int type, const shared_str& sect_name, CEffectorController*);
void RemoveEffector(CActor* A, int type);

class CEffectorController : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CEffectorController);

protected:
    CEffectorCam* m_ce{};
    CEffectorPP* m_pe{};

public:
    CEffectorController() = default;
    virtual ~CEffectorController();

    void SetPP(CEffectorPP* p) { m_pe = p; }
    void SetCam(CEffectorCam* p) { m_ce = p; }
    virtual BOOL Valid() { return m_ce || m_pe; }
    virtual float GetFactor() = 0;
};

class CAnimatorCamEffector : public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CAnimatorCamEffector, CEffectorCam);

public:
    bool m_bCyclic;

protected:
    using inherited = CEffectorCam;

    virtual bool Cyclic() const { return m_bCyclic; }
    CObjectAnimator* m_objectAnimator;

public:
    bool m_bAbsolutePositioning;
    float m_fov = 0.0f;

    CAnimatorCamEffector();
    virtual ~CAnimatorCamEffector();

    void Start(LPCSTR fn);
    virtual BOOL ProcessCam(SCamEffectorInfo& info);
    void SetCyclic(bool b) { m_bCyclic = b; }
    virtual BOOL Valid();
    float GetAnimatorLength() { return fLifeTime; }
    virtual bool AbsolutePositioning() { return m_bAbsolutePositioning; }
};

class CAnimatorCamEffectorScriptCB : public CAnimatorCamEffector
{
    RTTI_DECLARE_TYPEINFO(CAnimatorCamEffectorScriptCB, CAnimatorCamEffector);

public:
    shared_str cb_name;

protected:
    using inherited = CAnimatorCamEffector;

public:
    explicit CAnimatorCamEffectorScriptCB(LPCSTR _cb) : cb_name{_cb} {}

    virtual BOOL Valid();
    virtual BOOL AllowProcessingIfInvalid() { return m_bAbsolutePositioning; }
    virtual void ProcessIfInvalid(SCamEffectorInfo& info);
};

class CAnimatorCamLerpEffector : public CAnimatorCamEffector
{
    RTTI_DECLARE_TYPEINFO(CAnimatorCamLerpEffector, CAnimatorCamEffector);

protected:
    using inherited = CAnimatorCamEffector;
    GET_KOEFF_FUNC m_func;

public:
    void SetFactorFunc(GET_KOEFF_FUNC f) { m_func = f; }
    virtual BOOL ProcessCam(SCamEffectorInfo& info);
};

class CAnimatorCamLerpEffectorConst : public CAnimatorCamLerpEffector
{
    RTTI_DECLARE_TYPEINFO(CAnimatorCamLerpEffectorConst, CAnimatorCamLerpEffector);

protected:
    float m_factor;

public:
    CAnimatorCamLerpEffectorConst();

    void SetFactor(float v)
    {
        m_factor = v;
        clamp(m_factor, 0.0f, 1.0f);
    }
    float GetFactor() { return m_factor; }
};

class CCameraEffectorControlled : public CAnimatorCamLerpEffector
{
    RTTI_DECLARE_TYPEINFO(CCameraEffectorControlled, CAnimatorCamLerpEffector);

public:
    CEffectorController* m_controller;

    explicit CCameraEffectorControlled(CEffectorController* c);
    virtual ~CCameraEffectorControlled();

    virtual BOOL Valid();
};

class SndShockEffector : public CEffectorController
{
    RTTI_DECLARE_TYPEINFO(SndShockEffector, CEffectorController);

protected:
    using inherited = CEffectorController;

public:
    float m_snd_length; // ms
    float m_cur_length; // ms
    float m_stored_volume;
    float m_end_time;
    float m_life_time;
    CActor* m_actor;

public:
    SndShockEffector();
    virtual ~SndShockEffector();

    void Start(CActor* A, float snd_length, float power);
    void Update();

    virtual BOOL Valid();
    BOOL InWork();
    virtual float GetFactor();
};

//////////////////////////////////////////////////////////////////////////
class CControllerPsyHitCamEffector : public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CControllerPsyHitCamEffector, CEffectorCam);

protected:
    using inherited = CEffectorCam;

private:
    float m_time_total;
    float m_time_current{};
    Fvector m_dangle_target;
    Fvector m_dangle_current{};
    Fvector m_position_source;
    Fvector m_direction;
    float m_distance;
    float m_base_fov;
    float m_dest_fov;

public:
    explicit CControllerPsyHitCamEffector(const Fvector& src_pos, const Fvector& target_pos, float time, float base_fov, float dest_fov);

    [[nodiscard]] BOOL ProcessCam(SCamEffectorInfo& info) override;
};
//////////////////////////////////////////////////////////////////////////
