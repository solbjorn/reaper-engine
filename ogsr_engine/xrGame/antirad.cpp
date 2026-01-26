///////////////////////////////////////////////////////////////
// Antirad.cpp
// Antirad - таблетки выводящие радиацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "antirad.h"

#include "PhysicsShell.h"

CAntirad::CAntirad() = default;
CAntirad::~CAntirad() = default;

tmc::task<bool> CAntirad::net_Spawn(CSE_Abstract* DC) { co_return co_await inherited::net_Spawn(DC); }
void CAntirad::Load(LPCSTR section) { inherited::Load(section); }
tmc::task<void> CAntirad::net_Destroy() { co_await inherited::net_Destroy(); }

tmc::task<void> CAntirad::shedule_Update(u32 dt) { co_await inherited::shedule_Update(dt); }
tmc::task<void> CAntirad::UpdateCL() { co_await inherited::UpdateCL(); }

void CAntirad::OnH_A_Chield() { inherited::OnH_A_Chield(); }
void CAntirad::OnH_B_Independent(bool just_before_destroy) { inherited::OnH_B_Independent(just_before_destroy); }
