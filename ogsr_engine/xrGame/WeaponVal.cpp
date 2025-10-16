#include "stdafx.h"

#include "weaponval.h"

CWeaponVal::CWeaponVal() : CWeaponMagazined{SOUND_TYPE_WEAPON_SUBMACHINEGUN}
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponVal::~CWeaponVal() = default;

void CWeaponVal::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponVal>("CWeaponVal", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponVal>), "factory", &xr::client_factory<CWeaponVal>,
                                 sol::base_classes, xr::sol_bases<CWeaponVal>());
}
