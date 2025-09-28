#include "stdafx.h"

#include "weapongroza.h"

CWeaponGroza::CWeaponGroza(void) : CWeaponMagazinedWGrenade("GROZA", SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponGroza::~CWeaponGroza(void) {}

void CWeaponGroza::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponGroza>("CWeaponGroza", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponGroza>), "factory",
                                   &xr::client_factory<CWeaponGroza>, sol::base_classes, xr::sol_bases<CWeaponGroza>());
}
