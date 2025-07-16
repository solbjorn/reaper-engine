////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_movement_action.h"
#include "script_game_object.h"
#include "patrol_path_manager_space.h"
#include "detail_path_manager_space.h"
#include "ai_monster_space.h"
#include "patrol_path_params.h"
#include "patrol_path.h"

void CScriptMovementAction::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptMovementAction>(
        "move", sol::no_constructor, sol::call_constructor,
        sol::constructors<
            CScriptMovementAction(), CScriptMovementAction(const CScriptMovementAction::EInputKeys), CScriptMovementAction(const CScriptMovementAction::EInputKeys, float),
            CScriptMovementAction(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, CScriptGameObject*),
            CScriptMovementAction(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, CScriptGameObject*, float),
            CScriptMovementAction(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, const CPatrolPathParams&),
            CScriptMovementAction(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, const CPatrolPathParams&, float),
            CScriptMovementAction(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, const Fvector&),
            CScriptMovementAction(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, const Fvector&, float),
            CScriptMovementAction(const Fvector&, float),
            // Monsters
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, Fvector&), CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, CPatrolPathParams&),
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, CScriptGameObject*), CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, Fvector&, float),
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, u32, Fvector&), CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, u32, Fvector&, float),
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, CPatrolPathParams&, float),
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, CScriptGameObject*, float),
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, Fvector&, float, MonsterSpace::EScriptMonsterSpeedParam),
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, CPatrolPathParams&, float, MonsterSpace::EScriptMonsterSpeedParam),
            CScriptMovementAction(MonsterSpace::EScriptMonsterMoveAction, CScriptGameObject*, float, MonsterSpace::EScriptMonsterSpeedParam)>(),

        // body
        "crouch", sol::var(MonsterSpace::eBodyStateCrouch), "standing", sol::var(MonsterSpace::eBodyStateStand),
        // move
        "walk", sol::var(MonsterSpace::eMovementTypeWalk), "run", sol::var(MonsterSpace::eMovementTypeRun), "stand", sol::var(MonsterSpace::eMovementTypeStand),
        // path
        "line", sol::var(DetailPathManager::eDetailPathTypeSmooth), "dodge", sol::var(DetailPathManager::eDetailPathTypeSmoothDodge), "criteria",
        sol::var(DetailPathManager::eDetailPathTypeSmoothCriteria), "curve", sol::var(DetailPathManager::eDetailPathTypeSmooth), "curve_criteria",
        sol::var(DetailPathManager::eDetailPathTypeSmoothCriteria),
        // input
        "none", sol::var(CScriptMovementAction::eInputKeyNone), "fwd", sol::var(CScriptMovementAction::eInputKeyForward), "back", sol::var(CScriptMovementAction::eInputKeyBack),
        "left", sol::var(CScriptMovementAction::eInputKeyLeft), "right", sol::var(CScriptMovementAction::eInputKeyRight), "up", sol::var(CScriptMovementAction::eInputKeyShiftUp),
        "down", sol::var(CScriptMovementAction::eInputKeyShiftDown), "handbrake", sol::var(CScriptMovementAction::eInputKeyBreaks), "on",
        sol::var(CScriptMovementAction::eInputKeyEngineOn), "off", sol::var(CScriptMovementAction::eInputKeyEngineOff),
        // monster
        "walk_fwd", sol::var(MonsterSpace::eMA_WalkFwd), "walk_bkwd", sol::var(MonsterSpace::eMA_WalkBkwd), "run_fwd", sol::var(MonsterSpace::eMA_Run), "drag",
        sol::var(MonsterSpace::eMA_Drag), "jump", sol::var(MonsterSpace::eMA_Jump), "steal", sol::var(MonsterSpace::eMA_Steal),
        // monster_speed_param
        "default", sol::var(MonsterSpace::eSP_Default), "force", sol::var(MonsterSpace::eSP_ForceSpeed),

        "body", &CScriptMovementAction::SetBodyState, "move", &CScriptMovementAction::SetMovementType, "path", &CScriptMovementAction::SetPathType, "object",
        &CScriptMovementAction::SetObjectToGo, "patrol", &CScriptMovementAction::SetPatrolPath, "position", &CScriptMovementAction::SetPosition, "input",
        &CScriptMovementAction::SetInputKeys, "completed", sol::resolve<bool()>(&CScriptMovementAction::completed));
}
