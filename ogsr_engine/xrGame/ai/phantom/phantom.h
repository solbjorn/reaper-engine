#pragma once

#include "../../entity.h"
#include "../../../Include\xrRender\Kinematics.h"
#include "../../../Include\xrRender\animation_motion.h"

class CParticlesObject;

class CPhantom : public CEntity
{
    RTTI_DECLARE_TYPEINFO(CPhantom, CEntity);

private:
    typedef CEntity inherited;

    enum EState
    {
        stInvalid = -2,
        stIdle = -1,
        stBirth = 0,
        stFly = 1,
        stContact = 2,
        stShoot = 3,
        stCount
    };
    EState m_CurState;
    EState m_TgtState;

    void SwitchToState_internal(EState new_state);
    void SwitchToState(EState new_state) { m_TgtState = new_state; }
    void OnIdleState();
    void OnFlyState();
    void OnDeadState();

    void UpdateFlyMedia();

    CallMe::Delegate<void()> UpdateEvent;

    struct SStateData
    {
        shared_str particles;
        ref_sound sound;
        MotionID motion;
    };
    SStateData m_state_data[stCount];

private:
    CParticlesObject* m_fly_particles{};
    static void animation_end_callback(CBlend* B);

private:
    CObject* m_enemy{};

    float fSpeed;
    float fASpeed;
    Fvector2 vHP;

    float fContactHit;

    Fmatrix XFORM_center();

    CParticlesObject* PlayParticles(const shared_str& name, BOOL bAutoRemove, const Fmatrix& xform);
    //	void				PlayMotion					(MotionID);

    void UpdatePosition(const Fvector& tgt_pos);

    void PsyHit(const CObject* object, float value);

public:
    CPhantom();
    ~CPhantom() override;

    virtual void Load(LPCSTR section);
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();

    virtual void net_Export(CSE_Abstract* E);
    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);

    virtual void shedule_Update(u32 DT);
    tmc::task<void> UpdateCL() override;

    void HitSignal(float, Fvector&, CObject*, s16) override {}
    void HitImpulse(float, Fvector&, Fvector&) override {}
    virtual void Hit(SHit* pHDS);

    virtual BOOL IsVisibleForHUD() { return false; }
    virtual bool IsVisibleForZones() { return false; }

    virtual BOOL UsedAI_Locations() { return false; }

    virtual CEntity* cast_entity() { return this; }
};
