////////////////////////////////////////////////////////////////////////////
//	Module 		: script_entity_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script entity action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_entity_action.h"

void CScriptEntityAction::clone(const CScriptEntityAction& from)
{
    m_tMovementAction.clone(from.m_tMovementAction);
    m_tWatchAction.clone(from.m_tWatchAction);
    m_tAnimationAction.clone(from.m_tAnimationAction);
    m_tSoundAction.clone(from.m_tSoundAction);
    m_tParticleAction.clone(from.m_tParticleAction);
    m_tObjectAction.clone(from.m_tObjectAction);
    m_tActionCondition.clone(from.m_tActionCondition);
    m_tMonsterAction.clone(from.m_tMonsterAction);

    m_user_data = from.m_user_data;
    m_started = from.m_started;
}
