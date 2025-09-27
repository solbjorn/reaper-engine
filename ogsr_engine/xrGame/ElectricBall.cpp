///////////////////////////////////////////////////////////////
// ElectricBall.cpp
// ElectricBall - артефакт электрический шар
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ElectricBall.h"
#include "PhysicsShell.h"

CElectricBall::CElectricBall() = default;
CElectricBall::~CElectricBall() = default;

void CElectricBall::Load(LPCSTR section) { inherited::Load(section); }

void CElectricBall::UpdateCLChild()
{
    inherited::UpdateCLChild();

    if (H_Parent())
        XFORM().set(H_Parent()->XFORM());
}
