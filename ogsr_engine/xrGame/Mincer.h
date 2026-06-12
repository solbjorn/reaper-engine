/////////////////////////////////////////////////////
// Аномальная зона: "мясорубка"
// При попадании живого объекта в зону происходит
// электрический разряд
// Зона восстанавливает заряд через определенное время
// (через m_dwPeriod заряжается с 0 до m_fMaxPower)
//
/////////////////////////////////////////////////////

#pragma once

#include "gravizone.h"
#include "telewhirlwind.h"
#include "PhysicsShellHolder.h"
#include "script_export_space.h"
#include "PHDestroyable.h"

class CMincer : public CBaseGraviZone, public CPHDestroyableNotificator
{
    RTTI_DECLARE_TYPEINFO(CMincer, CBaseGraviZone, CPHDestroyableNotificator);

private:
    typedef CBaseGraviZone inherited;
    CTeleWhirlwind m_telekinetics;
    shared_str m_torn_particles;
    ref_sound m_tearing_sound;
    float m_fActorBlowoutRadiusPercent{0.5f};
    float m_fArtefactSpawnProbabilityTorn{}; // bak

public:
    [[nodiscard]] CTelekinesis& Telekinesis() override { return m_telekinetics; }

    CMincer();
    ~CMincer() override;

    void OnStateSwitch(EZoneState new_state) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;
    void feel_touch_new(CObject* O) override;
    void Load(gsl::czstring section) override;
    [[nodiscard]] bool BlowoutState() override;
    void AffectPullDead(CPhysicsShellHolder*, const Fvector&, float) override {}
    void AffectPullAlife(CEntityAlive* EA, const Fvector& throw_in_dir, float dist) override;
    void AffectThrow(SZoneObjectInfo* O, CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist) override;
    void ThrowInCenter(Fvector& C) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void Center(Fvector& C) const override;
    void NotificateDestroy(CPHDestroyableNotificate* dn) override;
    [[nodiscard]] f32 BlowoutRadiusPercent(CPhysicsShellHolder* GO) override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CMincer);

add_to_type_list(CMincer);
#undef script_type_list
#define script_type_list save_type_list(CMincer)
