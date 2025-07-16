#include "stdafx.h"
#include "weaponusp45.h"

CWeaponUSP45::CWeaponUSP45(void) : CWeaponPistol("USP")
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponUSP45::~CWeaponUSP45(void) {}

void CWeaponUSP45::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponUSP45>("CWeaponUSP45", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponUSP45>), sol::base_classes,
                                                  xr_sol_bases<CWeaponUSP45>());
}
