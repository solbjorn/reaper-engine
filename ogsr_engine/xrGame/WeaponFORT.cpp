#include "stdafx.h"
#include "WeaponFORT.h"

CWeaponFORT::CWeaponFORT() : CWeaponPistol("FORT") {}

CWeaponFORT::~CWeaponFORT() {}

void CWeaponFORT::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponFORT>("CWeaponFORT", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponFORT>), sol::base_classes,
                                  xr_sol_bases<CWeaponFORT>());
}
