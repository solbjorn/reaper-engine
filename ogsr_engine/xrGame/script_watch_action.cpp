////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_watch_action.h"
#include "script_game_object.h"
#include "sight_manager_space.h"

CScriptWatchAction::CScriptWatchAction() : m_tWatchType{SightManager::eSightTypeCurrentDirection} { m_bCompleted = true; }

CScriptWatchAction::~CScriptWatchAction() {}

void CScriptWatchAction::SetWatchObject(CScriptGameObject* tpObjectToWatch)
{
    m_tpObjectToWatch = tpObjectToWatch->operator CObject*();
    m_tGoalType = eGoalTypeObject;
    m_bCompleted = false;
}

void CScriptWatchAction::clone(const CScriptWatchAction& from)
{
    CScriptAbstractAction::clone(from);

    m_tpObjectToWatch = from.m_tpObjectToWatch;
    m_tWatchType = from.m_tWatchType;
    m_tGoalType = from.m_tGoalType;
    m_tWatchVector = from.m_tWatchVector;
    m_bone_to_watch = from.m_bone_to_watch;

    m_tTargetPoint = from.m_tTargetPoint;
    vel_bone_x = from.vel_bone_x;
    vel_bone_y = from.vel_bone_y;
}
