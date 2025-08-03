#include "stdafx.h"
#include "WeaponLR300.h"

CWeaponLR300::CWeaponLR300() : CWeaponMagazined("LR300", SOUND_TYPE_WEAPON_SUBMACHINEGUN) {}

CWeaponLR300::~CWeaponLR300() {}

void CWeaponLR300::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponLR300>("CWeaponLR300", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponLR300>), "factory",
                                   &client_factory<CWeaponLR300>, sol::base_classes, xr_sol_bases<CWeaponLR300>());
}
