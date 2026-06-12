///////////////////////////////////////////////////////////////
// GrenadeLauncher.h
// GrenadeLauncher - апгрейд оружия поствольный гранатомет
///////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item_object.h"

class CGrenadeLauncher : public CInventoryItemObject
{
    RTTI_DECLARE_TYPEINFO(CGrenadeLauncher, CInventoryItemObject);

private:
    typedef CInventoryItemObject inherited;

public:
    CGrenadeLauncher();
    ~CGrenadeLauncher() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void Load(gsl::czstring section) override;
    tmc::task<void> net_Destroy() override;

    void OnH_A_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;

    tmc::task<void> UpdateCL() override;

    [[nodiscard]] f32 GetGrenadeVel() { return m_fGrenadeVel; }

protected:
    // стартовая скорость вылета подствольной гранаты
    float m_fGrenadeVel;
};
