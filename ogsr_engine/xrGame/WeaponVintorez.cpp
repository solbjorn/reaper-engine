#include "stdafx.h"
#include "weaponvintorez.h"

CWeaponVintorez::CWeaponVintorez(void) : CWeaponMagazined("VINTOREZ", SOUND_TYPE_WEAPON_SNIPERRIFLE)
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponVintorez::~CWeaponVintorez(void) {}

void CWeaponVintorez::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponVintorez>("CWeaponVintorez", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponVintorez>),
                                                     sol::base_classes, xr_sol_bases<CWeaponVintorez>());
}
