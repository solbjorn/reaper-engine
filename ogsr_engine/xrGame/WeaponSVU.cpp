#include "stdafx.h"
#include "weaponsvu.h"

CWeaponSVU::CWeaponSVU(void) : CWeaponCustomPistol("SVU") {}

CWeaponSVU::~CWeaponSVU(void) {}

void CWeaponSVU::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponSVU>("CWeaponSVU", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponSVU>), "factory", &client_factory<CWeaponSVU>,
                                 sol::base_classes, xr_sol_bases<CWeaponSVU>());
}
