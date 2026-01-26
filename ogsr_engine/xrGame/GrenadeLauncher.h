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
    virtual void Load(LPCSTR section);
    tmc::task<void> net_Destroy() override;

    virtual void OnH_A_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);

    tmc::task<void> UpdateCL() override;

    float GetGrenadeVel() { return m_fGrenadeVel; }

protected:
    // стартовая скорость вылета подствольной гранаты
    float m_fGrenadeVel;
};
