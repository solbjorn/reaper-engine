///////////////////////////////////////////////////////////////
// Antirad.h
// Antirad - таблетки выводящие радиацию
///////////////////////////////////////////////////////////////

#pragma once

#include "eatable_item_object.h"

class CAntirad : public CEatableItemObject
{
    RTTI_DECLARE_TYPEINFO(CAntirad, CEatableItemObject);

private:
    typedef CEatableItemObject inherited;

public:
    CAntirad();
    ~CAntirad() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void Load(gsl::czstring section) override;
    tmc::task<void> net_Destroy() override;
    tmc::task<void> shedule_Update(u32 dt) override;
    tmc::task<void> UpdateCL() override;
    void OnH_A_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;
};
