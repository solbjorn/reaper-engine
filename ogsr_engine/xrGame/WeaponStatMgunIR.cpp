#include "stdafx.h"

#include "WeaponStatMgun.h"

#include "xr_level_controller.h"

void CWeaponStatMgun::OnMouseMove(int dx, int dy)
{
    if (Remote())
        return;

    float scale = psMouseSens * psMouseSensScale / 50.f;
    float h, p;
    m_destEnemyDir.getHP(h, p);
    if (dx)
    {
        float d = float(dx) * scale;
        h -= d;
        SetDesiredDir(h, p);
    }
    if (dy)
    {
        float d = ((psMouseInvert.test(1)) ? -1 : 1) * float(dy) * scale * 3.f / 4.f;
        p -= d;
        SetDesiredDir(h, p);
    }
}

void CWeaponStatMgun::OnKeyboardPress(EGameActions cmd)
{
    if (Remote())
        return;

    switch (cmd)
    {
    case EGameActions::kWPN_FIRE: FireStart(); break;
    default: break;
    }
}

void CWeaponStatMgun::OnKeyboardRelease(EGameActions cmd)
{
    if (Remote())
        return;

    switch (cmd)
    {
    case EGameActions::kWPN_FIRE: FireEnd(); break;
    default: break;
    }
}

void CWeaponStatMgun::OnKeyboardHold(EGameActions) {}
