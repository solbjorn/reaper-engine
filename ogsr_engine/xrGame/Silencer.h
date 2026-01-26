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
    virtual void Load(LPCSTR section);
    tmc::task<void> net_Destroy() override;

    virtual void OnH_A_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);

    tmc::task<void> UpdateCL() override;
};
