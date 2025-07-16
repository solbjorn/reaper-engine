#include "stdafx.h"
#include "weaponwalther.h"

CWeaponWalther::CWeaponWalther(void) : CWeaponPistol("WALTHER")
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponWalther::~CWeaponWalther(void) {}

void CWeaponWalther::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponWalther>("CWeaponWalther", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponWalther>),
                                                    sol::base_classes, xr_sol_bases<CWeaponWalther>());
}
