#include "stdafx.h"
#include "weapongroza.h"

CWeaponGroza::CWeaponGroza(void) : CWeaponMagazinedWGrenade("GROZA", SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponGroza::~CWeaponGroza(void) {}

void CWeaponGroza::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponGroza>("CWeaponGroza", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponGroza>), sol::base_classes,
                                                  xr_sol_bases<CWeaponGroza>());
}
