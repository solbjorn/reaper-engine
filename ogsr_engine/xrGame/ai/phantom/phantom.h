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

    tmc::task<void> SwitchToState_internal(EState new_state);
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

    void UpdatePosition(const Fvector& tgt_pos);

    void PsyHit(const CObject* object, float value);

public:
    CPhantom();
    ~CPhantom() override;

    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;

    void net_Export(CSE_Abstract* E) override;
    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

    tmc::task<void> shedule_Update(u32 DT) override;
    tmc::task<void> UpdateCL() override;

    void HitSignal(f32, Fvector3&, CObject*, s16) override {}
    void HitImpulse(f32, Fvector3&, Fvector3&) override {}
    void Hit(SHit* pHDS) override;

    [[nodiscard]] BOOL IsVisibleForHUD() override { return false; }
    [[nodiscard]] bool IsVisibleForZones() override { return false; }

    [[nodiscard]] BOOL UsedAI_Locations() override { return false; }
    [[nodiscard]] CEntity* cast_entity() override { return this; }
};
