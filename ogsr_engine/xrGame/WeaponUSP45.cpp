#include "stdafx.h"

#include "weaponusp45.h"

CWeaponUSP45::CWeaponUSP45() : CWeaponPistol{"USP"}
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponUSP45::~CWeaponUSP45() = default;

void CWeaponUSP45::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponUSP45>("CWeaponUSP45", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponUSP45>), "factory",
                                   &xr::client_factory<CWeaponUSP45>, sol::base_classes, xr::sol_bases<CWeaponUSP45>());
}
