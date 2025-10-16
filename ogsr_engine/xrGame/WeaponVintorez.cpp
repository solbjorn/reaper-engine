#include "stdafx.h"

#include "weaponvintorez.h"

CWeaponVintorez::CWeaponVintorez() : CWeaponMagazined{SOUND_TYPE_WEAPON_SNIPERRIFLE}
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponVintorez::~CWeaponVintorez() = default;

void CWeaponVintorez::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponVintorez>("CWeaponVintorez", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponVintorez>), "factory",
                                      &xr::client_factory<CWeaponVintorez>, sol::base_classes, xr::sol_bases<CWeaponVintorez>());
}
