#include "stdafx.h"

#include "WeaponPM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponPM::CWeaponPM() : CWeaponPistol{}
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponPM::~CWeaponPM() {}

void CWeaponPM::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponPM>("CWeaponPM", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponPM>), "factory", &xr::client_factory<CWeaponPM>,
                                sol::base_classes, xr::sol_bases<CWeaponPM>());
}
