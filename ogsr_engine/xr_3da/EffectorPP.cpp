// EffectorPP.cpp: implementation of the CEffectorFall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "EffectorPP.h"
#include "CameraManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorPP::CEffectorPP(EEffectorPPType type, float lifeTime, bool free_on_remove) : eType{type}, bFreeOnRemove{free_on_remove}, fLifeTime{lifeTime} {}
CEffectorPP::~CEffectorPP() {}

BOOL CEffectorPP::Process(SPPInfo&)
{
    fLifeTime -= Device.fTimeDelta;
    return TRUE;
}
