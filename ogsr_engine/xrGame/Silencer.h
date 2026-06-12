///////////////////////////////////////////////////////////////
// Silencer.h
// Silencer - апгрейд оружия глушитель
///////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item_object.h"

class CSilencer : public CInventoryItemObject
{
    RTTI_DECLARE_TYPEINFO(CSilencer, CInventoryItemObject);

private:
    typedef CInventoryItemObject inherited;

public:
    CSilencer();
    ~CSilencer() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void Load(gsl::czstring section) override;
    tmc::task<void> net_Destroy() override;

    void OnH_A_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;

    tmc::task<void> UpdateCL() override;
};
