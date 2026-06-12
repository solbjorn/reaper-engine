#pragma once

#include "../BaseMonster/base_monster.h"
#include "script_export_space.h"

class CCat : public CBaseMonster
{
    RTTI_DECLARE_TYPEINFO(CCat, CBaseMonster);

public:
    typedef CBaseMonster inherited;

    CCat();
    ~CCat() override;

    void Load(gsl::czstring section) override;
    void reinit() override;

    tmc::task<void> UpdateCL() override;

    void CheckSpecParams(u32 spec_params) override;

    void try_to_jump();
    void HitEntityInJump(const CEntity* pEntity) override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CCat);

add_to_type_list(CCat);
#undef script_type_list
#define script_type_list save_type_list(CCat)
