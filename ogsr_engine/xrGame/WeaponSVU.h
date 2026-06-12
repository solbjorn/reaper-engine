#pragma once

#include "weaponcustompistol.h"
#include "script_export_space.h"

class CWeaponSVU : public CWeaponCustomPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponSVU, CWeaponCustomPistol);

public:
    typedef CWeaponCustomPistol inherited;

    CWeaponSVU();
    ~CWeaponSVU() override;

protected:
    // Вообще СВУ короткая, мб и не надо её относить к длинным снайперкам типа СВД
    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return SniperRifle; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponSVU);

add_to_type_list(CWeaponSVU);
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVU)
