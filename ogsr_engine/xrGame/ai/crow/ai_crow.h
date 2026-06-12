////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.h
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_CROW__
#define __XRAY_AI_CROW__

#include "../../entity_alive.h"
#include "../../../Include/xrRender/KinematicsAnimated.h"

class CAI_Crow : public CEntity
{
    RTTI_DECLARE_TYPEINFO(CAI_Crow, CEntity);

private:
    typedef CEntity inherited;
    enum ECrowStates
    {
        eUndef = -1,
        eDeathFall = 0,
        eDeathDead,
        eFlyIdle,
        eFlyUp
    };

    // constants and types
    enum
    {
        MAX_ANIM_COUNT = 8
    };
    enum
    {
        MAX_SND_COUNT = 8
    };

    // animations
    struct SAnim
    {
        typedef svector<MotionID, MAX_ANIM_COUNT> MotionSVec;
        MotionSVec m_Animations;
        const MotionID& GetRandom() { return m_Animations[Random.randI(0, m_Animations.size())]; }
        void Load(IKinematicsAnimated* visual, LPCSTR prefix);
    };

    struct SSound
    {
        typedef svector<ref_sound, MAX_SND_COUNT> SoundSVec;
        SoundSVec m_Sounds;
        ref_sound& GetRandom() { return m_Sounds[Random.randI(0, m_Sounds.size())]; }
        void Load(LPCSTR prefix);
        void SetPosition(const Fvector& pos);
        void Unload();
    };

public:
    void OnHitEndPlaying();

protected:
    struct SCrowAnimations
    {
        SAnim m_idle;
        SAnim m_fly;
        SAnim m_death;
        SAnim m_death_idle;
        SAnim m_death_dead;
    };
    SCrowAnimations m_Anims;
    struct SCrowSounds
    {
        SSound m_idle;
    };
    SCrowSounds m_Sounds;

    Fvector vOldPosition;
    ECrowStates st_current, st_target;
    // parameters block
    Fvector vGoalDir;
    Fvector vCurrentDir;
    Fvector vHPB;
    float fDHeading;

    // constants
    float fGoalChangeDelta;
    float fSpeed;
    float fASpeed;
    float fMinHeight;
    Fvector vVarGoal;
    float fIdleSoundDelta;

    // variables
    float fGoalChangeTime;
    float fIdleSoundTime;

    //
    bool bPlayDeathIdle;

    void switch2_FlyUp();
    void switch2_FlyIdle();
    void switch2_DeathFall();
    void switch2_DeathDead();

    void state_DeathFall();
    void state_Flying(float dt);

    void CreateSkeleton();

    void UpdateWorkload(float DT);

public:
    u32 o_workload_frame;
    u32 o_workload_rframe;

public:
    CAI_Crow();
    ~CAI_Crow() override;

    void Load(gsl::czstring section) override;
    void init();
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    [[nodiscard]] BOOL renderable_ShadowGenerate() override { return FALSE; }
    [[nodiscard]] BOOL renderable_ShadowReceive() override { return FALSE; }
    void renderable_Render(u32 context_id, IRenderable* root) override;
    tmc::task<void> shedule_Update(u32 DT) override;
    tmc::task<void> UpdateCL() override;

    [[nodiscard]] CEntity* cast_entity() override { return this; }

    void net_Export(CSE_Abstract* E) override;

    void g_fireParams(CHudItem*, Fvector3&, Fvector3&, const bool = false) override {}
    virtual void g_WeaponBones(s32&, s32&, s32&) {}

    void HitSignal(f32, Fvector3&, CObject*, s16) override;
    void HitImpulse(f32, Fvector3&, Fvector3&) override;
    void Hit(SHit* pHDS) override;
    tmc::task<void> Die(CObject* who) override;
    [[nodiscard]] virtual f32 ffGetFov() const { return 150.f; }
    [[nodiscard]] virtual f32 ffGetRange() const { return 30.f; }

    [[nodiscard]] BOOL IsVisibleForHUD() override { return FALSE; }
    [[nodiscard]] bool IsVisibleForZones() override { return false; }
    [[nodiscard]] BOOL UsedAI_Locations() override;
    void create_physic_shell() override;
};

#endif
