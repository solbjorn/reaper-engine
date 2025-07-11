#pragma once
#include "game_cl_base.h"

class game_cl_Single : public game_cl_GameState
{
    RTTI_DECLARE_TYPEINFO(game_cl_Single, game_cl_GameState);

public:
    game_cl_Single();
    virtual CUIGameCustom* createGameUI();
    virtual bool IsServerControlHits() { return true; };

    virtual ALife::_TIME_ID GetEnvironmentGameTime() { return GetGameTime(); };
    virtual float GetEnvironmentGameTimeFactor() { return GetGameTimeFactor(); };

    void OnDifficultyChanged();
};

// game difficulty
enum ESingleGameDifficulty
{
    egdNovice = 0,
    egdStalker = 1,
    egdVeteran = 2,
    egdMaster = 3,
    egdCount,
    egd_force_u32 = u32(-1)
};

extern ESingleGameDifficulty g_SingleGameDifficulty;
extern xr_token difficulty_type_token[];

typedef enum_exporter<ESingleGameDifficulty> CScriptGameDifficulty;
add_to_type_list(CScriptGameDifficulty)
#undef script_type_list
#define script_type_list save_type_list(CScriptGameDifficulty)
