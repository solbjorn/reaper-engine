///////////////////////////////////////////////////////////////
// Medkit.cpp
// Medkit - аптечка, повышающая здоровье
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "medkit.h"

#include "PhysicsShell.h"

CMedkit::CMedkit() = default;
CMedkit::~CMedkit() = default;

tmc::task<bool> CMedkit::net_Spawn(CSE_Abstract* DC) { co_return co_await inherited::net_Spawn(DC); }
void CMedkit::Load(LPCSTR section) { inherited::Load(section); }
tmc::task<void> CMedkit::net_Destroy() { co_await inherited::net_Destroy(); }

tmc::task<void> CMedkit::shedule_Update(u32 dt) { co_await inherited::shedule_Update(dt); }
tmc::task<void> CMedkit::UpdateCL() { co_await inherited::UpdateCL(); }

void CMedkit::OnH_A_Chield() { inherited::OnH_A_Chield(); }
void CMedkit::OnH_B_Independent(bool just_before_destroy) { inherited::OnH_B_Independent(just_before_destroy); }
