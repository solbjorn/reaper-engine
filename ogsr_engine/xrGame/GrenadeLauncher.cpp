///////////////////////////////////////////////////////////////
// GrenadeLauncher.cpp
// GrenadeLauncher - апгрейд оружия поствольный гранатомет
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "grenadelauncher.h"

CGrenadeLauncher::CGrenadeLauncher() { m_fGrenadeVel = 0.f; }
CGrenadeLauncher::~CGrenadeLauncher() {}

tmc::task<bool> CGrenadeLauncher::net_Spawn(CSE_Abstract* DC) { co_return co_await inherited::net_Spawn(DC); }

void CGrenadeLauncher::Load(LPCSTR section)
{
    m_fGrenadeVel = pSettings->r_float(section, "grenade_vel");
    inherited::Load(section);
}

tmc::task<void> CGrenadeLauncher::net_Destroy() { co_await inherited::net_Destroy(); }
tmc::task<void> CGrenadeLauncher::UpdateCL() { co_await inherited::UpdateCL(); }

void CGrenadeLauncher::OnH_A_Chield() { inherited::OnH_A_Chield(); }
void CGrenadeLauncher::OnH_B_Independent(bool just_before_destroy) { inherited::OnH_B_Independent(just_before_destroy); }
