#pragma once

enum EGameTypes
{
    GAME_ANY = 0,
    GAME_SINGLE = 1,

    // identifiers in range [100...254] are registered for script game type
    GAME_DUMMY = 255 // temporary game type
};

enum EGamePlayerFlags : u32
{
    GAME_PLAYER_FLAG_LOCAL = (1 << 0),
};

enum EGamePhases : u32
{
    GAME_PHASE_NONE = 0,
    GAME_PHASE_INPROGRESS,
    GAME_PHASE_PENDING,
};
