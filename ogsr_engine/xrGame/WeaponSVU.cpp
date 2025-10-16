#include "stdafx.h"

#include "weaponsvu.h"

CWeaponSVU::CWeaponSVU() : CWeaponCustomPistol{} {}
CWeaponSVU::~CWeaponSVU() {}

void CWeaponSVU::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponSVU>("CWeaponSVU", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponSVU>), "factory", &xr::client_factory<CWeaponSVU>,
                                 sol::base_classes, xr::sol_bases<CWeaponSVU>());
}
