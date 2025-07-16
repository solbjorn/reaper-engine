#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"

#include "game_cl_single.h"
#include "UIGameSP.h"
#include "clsid_game.h"
#include "actor.h"

ESingleGameDifficulty g_SingleGameDifficulty = egdMaster;

xr_token difficulty_type_token[] = {{"gd_novice", egdNovice}, {"gd_stalker", egdStalker}, {"gd_veteran", egdVeteran}, {"gd_master", egdMaster}, {nullptr, 0}};

game_cl_Single::game_cl_Single() {}

CUIGameCustom* game_cl_Single::createGameUI()
{
    CLASS_ID clsid = CLSID_GAME_UI_SINGLE;
    CUIGameSP* pUIGame = smart_cast<CUIGameSP*>(NEW_INSTANCE(clsid));
    R_ASSERT(pUIGame);
    pUIGame->SetClGame(this);
    pUIGame->Init();
    return pUIGame;
}

void game_cl_Single::OnDifficultyChanged() { Actor()->OnDifficultyChanged(); }

template <>
void CScriptGameDifficulty::script_register(lua_State* L)
{
    sol::state_view(L).new_enum("game_difficulty", "novice", egdNovice, "stalker", egdStalker, "veteran", egdVeteran, "master", egdMaster);
}
