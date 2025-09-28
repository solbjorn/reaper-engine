#include "stdafx.h"

#include "weaponwalther.h"

CWeaponWalther::CWeaponWalther(void) : CWeaponPistol("WALTHER")
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponWalther::~CWeaponWalther(void) {}

void CWeaponWalther::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponWalther>("CWeaponWalther", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponWalther>), "factory",
                                     &xr::client_factory<CWeaponWalther>, sol::base_classes, xr::sol_bases<CWeaponWalther>());
}
