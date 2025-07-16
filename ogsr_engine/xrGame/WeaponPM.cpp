#include "stdafx.h"
#include "WeaponPM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponPM::CWeaponPM() : CWeaponPistol("PM")
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponPM::~CWeaponPM() {}

void CWeaponPM::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponPM>("CWeaponPM", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponPM>), sol::base_classes,
                                               xr_sol_bases<CWeaponPM>());
}
