///////////////////////////////////////////////////////////////
// Silencer.cpp
// Silencer - апгрейд оружия глушитель
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "silencer.h"

CSilencer::CSilencer() = default;
CSilencer::~CSilencer() = default;

tmc::task<bool> CSilencer::net_Spawn(CSE_Abstract* DC) { co_return co_await inherited::net_Spawn(DC); }
void CSilencer::Load(LPCSTR section) { inherited::Load(section); }
tmc::task<void> CSilencer::net_Destroy() { co_await inherited::net_Destroy(); }

tmc::task<void> CSilencer::UpdateCL() { co_await inherited::UpdateCL(); }

void CSilencer::OnH_A_Chield() { inherited::OnH_A_Chield(); }
void CSilencer::OnH_B_Independent(bool just_before_destroy) { inherited::OnH_B_Independent(just_before_destroy); }
