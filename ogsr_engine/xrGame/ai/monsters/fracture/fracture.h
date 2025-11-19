#pragma once

#include "../BaseMonster/base_monster.h"
#include "script_export_space.h"

class CStateManagerFracture;

class CFracture : public CBaseMonster
{
    RTTI_DECLARE_TYPEINFO(CFracture, CBaseMonster);

public:
    typedef CBaseMonster inherited;

    CFracture();
    ~CFracture() override;

    virtual void Load(LPCSTR section);
    virtual void CheckSpecParams(u32 spec_params);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CFracture);

add_to_type_list(CFracture);
#undef script_type_list
#define script_type_list save_type_list(CFracture)
