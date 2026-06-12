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
    ~CWeaponCustomPistol() override;

    [[nodiscard]] s32 GetCurrentFireMode() override { return m_bHasDifferentFireModes ? m_aFireModes[m_iCurFireMode] : 1; }

protected:
    void switch2_Fire() override;
};
