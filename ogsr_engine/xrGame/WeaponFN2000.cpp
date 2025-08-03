#include "stdafx.h"
#include "WeaponFN2000.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponFN2000::CWeaponFN2000() : CWeaponMagazined("FN2000", SOUND_TYPE_WEAPON_SNIPERRIFLE)
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponFN2000::~CWeaponFN2000() {}

void CWeaponFN2000::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponFN2000>("CWeaponFN2000", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponFN2000>), sol::base_classes,
                                    xr_sol_bases<CWeaponFN2000>());
}
