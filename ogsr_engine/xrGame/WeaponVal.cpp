#include "stdafx.h"
#include "weaponval.h"

CWeaponVal::CWeaponVal(void) : CWeaponMagazined("VAL", SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponVal::~CWeaponVal(void) {}

void CWeaponVal::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponVal>("CWeaponVal", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponVal>), sol::base_classes,
                                                xr_sol_bases<CWeaponVal>());
}
