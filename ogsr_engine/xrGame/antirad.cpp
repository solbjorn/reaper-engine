///////////////////////////////////////////////////////////////
// Antirad.cpp
// Antirad - таблетки выводящие радиацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "antirad.h"

#include "PhysicsShell.h"

CAntirad::CAntirad() = default;
CAntirad::~CAntirad() = default;

BOOL CAntirad::net_Spawn(CSE_Abstract* DC) { return (inherited::net_Spawn(DC)); }
void CAntirad::Load(LPCSTR section) { inherited::Load(section); }
void CAntirad::net_Destroy() { inherited::net_Destroy(); }

void CAntirad::shedule_Update(u32 dt) { inherited::shedule_Update(dt); }
tmc::task<void> CAntirad::UpdateCL() { co_await inherited::UpdateCL(); }

void CAntirad::OnH_A_Chield() { inherited::OnH_A_Chield(); }
void CAntirad::OnH_B_Independent(bool just_before_destroy) { inherited::OnH_B_Independent(just_before_destroy); }
