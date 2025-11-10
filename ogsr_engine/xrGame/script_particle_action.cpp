////////////////////////////////////////////////////////////////////////////
//	Module 		: script_particle_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script particle action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_particle_action.h"
#include "particlesobject.h"

CScriptParticleAction::~CScriptParticleAction()
{
    // xr_delete			(m_tpParticleSystem);
}

void CScriptParticleAction::SetParticle(LPCSTR caParticleToRun, bool bAutoRemove)
{
    m_caParticleToRun._set(caParticleToRun);
    m_tGoalType = eGoalTypeParticleAttached;
    m_tpParticleSystem = CParticlesObject::Create(*m_caParticleToRun, BOOL(m_bAutoRemove = bAutoRemove));
    m_bStartedToPlay = false;
    m_bCompleted = false;
}

void CScriptParticleAction::clone(const CScriptParticleAction& from)
{
    CScriptAbstractAction::clone(from);

    m_caParticleToRun = from.m_caParticleToRun;
    m_caBoneName = from.m_caBoneName;
    m_tGoalType = from.m_tGoalType;
    m_tpParticleSystem = from.m_tpParticleSystem;
    m_bStartedToPlay = from.m_bStartedToPlay;
    m_tParticlePosition = from.m_tParticlePosition;
    m_tParticleAngles = from.m_tParticleAngles;
    m_tParticleVelocity = from.m_tParticleVelocity;
    m_bAutoRemove = from.m_bAutoRemove;
}
