///////////////////////////////////////////////////////////////
// BastArtifact.h
// BastArtefact - артефакт мочалка
///////////////////////////////////////////////////////////////

#pragma once

#include "artifact.h"
#include "..\xr_3da\feel_touch.h"
#include "entity_alive.h"

struct SGameMtl;
struct dContact;

DEFINE_VECTOR(CEntityAlive*, ALIVE_LIST, ALIVE_LIST_it);

class CBastArtefact : public CArtefact, public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(CBastArtefact, CArtefact, Feel::Touch);

private:
    typedef CArtefact inherited;

public:
    CBastArtefact();
    ~CBastArtefact() override;

    void Load(gsl::czstring section) override;
    tmc::task<void> shedule_Update(u32 dt) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;

    void Hit(SHit* pHDS) override;

    [[nodiscard]] bool Useful() const override;

    void feel_touch_new(CObject* O) override;
    void feel_touch_delete(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;

    [[nodiscard]] bool IsAttacking() { return !!m_AttakingEntity; }

protected:
    void UpdateCLChild() override;

    static void ObjectContactCallback(bool&, bool, dContact& c, SGameMtl*, SGameMtl*);
    // столкновение мочалки с сущностью
    void BastCollision(CEntityAlive* pEntityAlive);

    // параметры артефакта

    // пороговое значение импульса после получения
    // которого артефакт активизируется
    float m_fImpulseThreshold;

    float m_fEnergy;
    float m_fEnergyMax;
    float m_fEnergyDecreasePerTime;
    shared_str m_sParticleName;

    float m_fRadius;
    float m_fStrikeImpulse;

    // флаг, того что артефакт получил хит
    // и теперь может совершить бросок
    bool m_bStrike{};

    // список живых существ в зоне досягаемости артефакта
    ALIVE_LIST m_AliveList;
    // то, что мы ударили
    CEntityAlive* m_pHitedEntity{};
    // то что атакуем
    CEntityAlive* m_AttakingEntity{};

public:
    void setup_physic_shell() override;
};
XR_SOL_BASE_CLASSES(CBastArtefact);
