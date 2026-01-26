///////////////////////////////////////////////////////////////
// Medkit.h
// Medkit - аптечка, повышающая здоровье
///////////////////////////////////////////////////////////////

#pragma once

#include "eatable_item_object.h"

class CMedkit : public CEatableItemObject
{
    RTTI_DECLARE_TYPEINFO(CMedkit, CEatableItemObject);

private:
    typedef CEatableItemObject inherited;

public:
    CMedkit();
    ~CMedkit() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    virtual void Load(LPCSTR section);
    tmc::task<void> net_Destroy() override;
    tmc::task<void> shedule_Update(u32 dt) override;
    tmc::task<void> UpdateCL() override;
    virtual void OnH_A_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);
};
