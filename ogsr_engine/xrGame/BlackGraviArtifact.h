///////////////////////////////////////////////////////////////
// BlackGraviArtifact.h
// BlackGraviArtefact - гравитационный артефакт,
// такой же как и обычный, но при получении хита
///////////////////////////////////////////////////////////////

#pragma once

#include "GraviArtifact.h"
#include "..\xr_3da\feel_touch.h"
#include "PhysicsShellHolder.h"

DEFINE_VECTOR(CPhysicsShellHolder*, GAME_OBJECT_LIST, GAME_OBJECT_LIST_it);

class CBlackGraviArtefact : public CGraviArtefact, public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(CBlackGraviArtefact, CGraviArtefact, Feel::Touch);

private:
    collide::rq_results rq_storage;

    typedef CGraviArtefact inherited;

public:
    CBlackGraviArtefact();
    ~CBlackGraviArtefact() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;

    void Load(gsl::czstring section) override;

    void Hit(SHit* pHDS) override;

    void feel_touch_new(CObject* O) override;
    void feel_touch_delete(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;

protected:
    void net_Relcase(CObject* O) override;
    void UpdateCLChild() override;

    // гравитационный удар по всем объектам в зоне досягаемости
    void GraviStrike();

    GAME_OBJECT_LIST m_GameObjectList;

    // которого артефакт активизируется
    float m_fImpulseThreshold;
    // радиус действия артефакта
    float m_fRadius;
    // импульс передаваемый окружающим предметам
    float m_fStrikeImpulse;

    // флаг, того что артефакт получил хит
    // и теперь может совершить бросок
    bool m_bStrike;

    shared_str m_sParticleName;
};
XR_SOL_BASE_CLASSES(CBlackGraviArtefact);
