////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_movement_action.h"
#include "script_game_object.h"
#include "detail_path_manager_space.h"
#include "patrol_path_manager_space.h"
#include "patrol_path_params.h"
#include "ai_monster_space.h"

CScriptMovementAction::CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams& tPatrolPathParams, float dist_to_end,
                                             MonsterSpace::EScriptMonsterSpeedParam speed_param)
{
    m_tMoveAction = tAct;
    SetPatrolPath(tPatrolPathParams.m_path, tPatrolPathParams.m_path_name);
    SetPatrolStart(tPatrolPathParams.m_tPatrolPathStart);
    SetPatrolStop(tPatrolPathParams.m_tPatrolPathStop);
    SetPatrolRandom(tPatrolPathParams.m_bRandom);
    m_tSpeedParam = speed_param;
    m_previous_patrol_point = tPatrolPathParams.m_previous_index;
    m_fDistToEnd = dist_to_end;
}

CScriptMovementAction::CScriptMovementAction(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType,
                                             const CPatrolPathParams& tPatrolPathParams, float fSpeed)
{
    SetBodyState(tBodyState);
    SetMovementType(tMovementType);
    SetPathType(tPathType);
    SetPatrolPath(tPatrolPathParams.m_path, tPatrolPathParams.m_path_name);
    SetPatrolStart(tPatrolPathParams.m_tPatrolPathStart);
    SetPatrolStop(tPatrolPathParams.m_tPatrolPathStop);
    SetPatrolRandom(tPatrolPathParams.m_bRandom);
    SetSpeed(fSpeed);
}

CScriptMovementAction::CScriptMovementAction()
{
    SetInputKeys(eInputKeyNone);
    SetBodyState(MonsterSpace::eBodyStateStand);
    SetMovementType(MonsterSpace::eMovementTypeStand);
    SetPathType(DetailPathManager::eDetailPathTypeSmooth);
    SetPatrolPath(nullptr, shared_str{""});
    SetPatrolStart(PatrolPathManager::ePatrolStartTypeNearest);
    SetPatrolStop(PatrolPathManager::ePatrolRouteTypeContinue);
    SetPatrolRandom(true);
    SetSpeed(0);
    SetObjectToGo(nullptr);
    SetPosition(Fvector{});
    m_tGoalType = eGoalTypeDummy;
    m_bCompleted = true;
}

CScriptMovementAction::CScriptMovementAction(const Fvector& tPosition, float fSpeed)
{
    SetBodyState(MonsterSpace::eBodyStateStand);
    SetMovementType(MonsterSpace::eMovementTypeStand);
    SetPathType(DetailPathManager::eDetailPathTypeSmooth);
    SetPosition(tPosition);
    SetSpeed(fSpeed);
    m_tGoalType = eGoalTypeNoPathPosition;
}

CScriptMovementAction::CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction tAct, Fvector& tPosition, float dist_to_end)
{
    MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default;
    m_tMoveAction = tAct;
    SetPosition(tPosition);
    m_tSpeedParam = speed_param;
    m_fDistToEnd = dist_to_end;
}

CScriptMovementAction::CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction tAct, u32 node_id, Fvector& tPosition, float dist_to_end)
{
    m_tMoveAction = tAct;
    m_tDestinationPosition = tPosition;
    m_tGoalType = eGoalTypePathNodePosition;
    m_tSpeedParam = MonsterSpace::eSP_Default;
    m_fDistToEnd = dist_to_end;
    m_tNodeID = node_id;
    m_bCompleted = false;
}

CScriptMovementAction::CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams& tPatrolPathParams, float dist_to_end)
{
    MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default;
    m_tMoveAction = tAct;
    SetPatrolPath(tPatrolPathParams.m_path, tPatrolPathParams.m_path_name);
    SetPatrolStart(tPatrolPathParams.m_tPatrolPathStart);
    SetPatrolStop(tPatrolPathParams.m_tPatrolPathStop);
    SetPatrolRandom(tPatrolPathParams.m_bRandom);
    m_tSpeedParam = speed_param;
    m_previous_patrol_point = tPatrolPathParams.m_previous_index;
    m_fDistToEnd = dist_to_end;
}

CScriptMovementAction::CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction tAct, CScriptGameObject* tpObjectToGo, float dist_to_end)
{
    MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default;
    m_tMoveAction = tAct;
    SetObjectToGo(tpObjectToGo);
    m_tSpeedParam = speed_param;
    m_fDistToEnd = dist_to_end;
}

CScriptMovementAction::~CScriptMovementAction() {}

void CScriptMovementAction::SetObjectToGo(CScriptGameObject* tpObjectToGo)
{
    if (tpObjectToGo)
        m_tpObjectToGo = tpObjectToGo->operator CObject*();
    else
        m_tpObjectToGo = nullptr;

    m_tGoalType = eGoalTypeObject;
    m_bCompleted = false;
}

void CScriptMovementAction::clone(const CScriptMovementAction& from)
{
    CScriptAbstractAction::clone(from);

    m_path_name = from.m_path_name;
    m_tBodyState = from.m_tBodyState;
    m_tMovementType = from.m_tMovementType;
    m_tPathType = from.m_tPathType;
    m_tpObjectToGo = from.m_tpObjectToGo;
    m_path = from.m_path;
    m_tPatrolPathStart = from.m_tPatrolPathStart;
    m_tPatrolPathStop = from.m_tPatrolPathStop;
    m_tDestinationPosition = from.m_tDestinationPosition;
    m_tNodeID = from.m_tNodeID;
    m_tGoalType = from.m_tGoalType;
    m_fSpeed = from.m_fSpeed;
    m_bRandom = from.m_bRandom;
    m_tInputKeys = from.m_tInputKeys;
    m_tMoveAction = from.m_tMoveAction;
    m_tSpeedParam = from.m_tSpeedParam;
    m_previous_patrol_point = from.m_previous_patrol_point;
    m_fDistToEnd = from.m_fDistToEnd;
}
