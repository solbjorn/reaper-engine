#include "stdafx.h"

#include "weaponsvd.h"
#include "game_object_space.h"

CWeaponSVD::CWeaponSVD() : CWeaponCustomPistol{} {}
CWeaponSVD::~CWeaponSVD() {}

void CWeaponSVD::switch2_Fire()
{
    // SetPending(TRUE);
    inherited::switch2_Fire();
}

void CWeaponSVD::OnAnimationEnd(u32 state) { inherited::OnAnimationEnd(state); }

void CWeaponSVD::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponSVD>("CWeaponSVD", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponSVD>), "factory", &xr::client_factory<CWeaponSVD>,
                                 sol::base_classes, xr::sol_bases<CWeaponSVD>());
}
