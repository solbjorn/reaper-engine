//////////////////////////////////////////////////////////////////////////
// GraviZone.h:		гравитационная аномалия
//////////////////////////////////////////////////////////////////////////
//					состоит как бы из 2х зон
//					одна затягивает объект, другая взрывает и
//					все неживые объекты (предметы и трупы)
//					поднимает в воздух и качает там какое-то
//					время
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "customzone.h"
#include "ai/monsters/telekinesis.h"

class XR_NOVTABLE CBaseGraviZone : public CCustomZone
{
    RTTI_DECLARE_TYPEINFO(CBaseGraviZone, CCustomZone);

private:
    typedef CCustomZone inherited;

public:
    CBaseGraviZone();
    ~CBaseGraviZone() override;

    void Load(gsl::czstring section) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Relcase(CObject* O) override;

    // воздействие зоной на объект
    void Affect(SZoneObjectInfo* O) override;
    virtual void AffectPull(CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist);
    virtual void AffectPullAlife(CEntityAlive* EA, const Fvector& throw_in_dir, float dist);
    virtual void AffectPullDead(CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist);
    virtual void AffectThrow(SZoneObjectInfo* O, CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist);
    virtual void ThrowInCenter(Fvector& C);
    virtual bool CheckAffectField(CPhysicsShellHolder* GO, float dist_to_radius);
    tmc::task<void> shedule_Update(u32 dt) override;
    [[nodiscard]] bool BlowoutState() override;
    [[nodiscard]] bool IdleState() override;

    void exit_Zone(SZoneObjectInfo& io) override;

    [[nodiscard]] f32 RelativePower(f32 dist) override;
    [[nodiscard]] virtual f32 BlowoutRadiusPercent(CPhysicsShellHolder*) { return m_fBlowoutRadiusPercent; }

protected:
    [[nodiscard]] virtual CTelekinesis& Telekinesis() = 0;

protected:
    // сила импульса втягивания в зону (для веса 100 кг)
    float m_fThrowInImpulse;
    // сила импульса втягивания в зону для живых существ
    float m_fThrowInImpulseAlive;
    // коэфф. затягивания (чем меньше, тем плавнее затягивает)
    float m_fThrowInAtten;
    // радиус действия выброса (в процентах от всего)
    float m_fBlowoutRadiusPercent;

    // параметры телекинеза
    float m_fTeleHeight;
    u32 m_dwTimeToTele;
    u32 m_dwTelePause;
    u32 m_dwTeleTime{};

    // имя партиклов телекинеза
    void PlayTeleParticles(CGameObject* pObject);
    void StopTeleParticles(CGameObject* pObject);

    shared_str m_sTeleParticlesBig;
    shared_str m_sTeleParticlesSmall;
};
