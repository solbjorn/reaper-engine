///////////////////////////////////////////////////////////////
// FadedBall.cpp
// FadedBall - артефакт блеклый шар
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "FadedBall.h"
#include "PhysicsShell.h"

CFadedBall::CFadedBall() = default;
CFadedBall::~CFadedBall() = default;

void CFadedBall::Load(LPCSTR section) { inherited::Load(section); }
