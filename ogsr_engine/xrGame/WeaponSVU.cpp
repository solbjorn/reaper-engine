#include "stdafx.h"
#include "weaponsvu.h"

CWeaponSVU::CWeaponSVU(void) : CWeaponCustomPistol("SVU") {}

CWeaponSVU::~CWeaponSVU(void) {}

void CWeaponSVU::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponSVU>("CWeaponSVU", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponSVU>), sol::base_classes,
                                                xr_sol_bases<CWeaponSVU>());
}
