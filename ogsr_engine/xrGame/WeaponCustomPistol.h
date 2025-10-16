#pragma once

#include "WeaponMagazined.h"

#define SND_RIC_COUNT 5

class CWeaponCustomPistol : public CWeaponMagazined
{
    RTTI_DECLARE_TYPEINFO(CWeaponCustomPistol, CWeaponMagazined);

private:
    typedef CWeaponMagazined inherited;

public:
    CWeaponCustomPistol();
    virtual ~CWeaponCustomPistol();

    virtual int GetCurrentFireMode() { return m_bHasDifferentFireModes ? m_aFireModes[m_iCurFireMode] : 1; }

protected:
    virtual void switch2_Fire();
};
