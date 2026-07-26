#pragma once

#include "../xr_3da/NET_Server_Trash/NET_utils.h"

struct GameEvent final
{
    u16 type;
    u32 time;
    ClientID sender;
    NET_Packet P;
};

class GameEventQueue final
{
    xrCriticalSection cs;
    xr_deque<GameEvent*> ready;
    xr_vector<GameEvent*> unused;

public:
    GameEventQueue();
    ~GameEventQueue();

    GameEvent* Create();
    GameEvent* Create(NET_Packet& P, u16 type, u32 time, ClientID clientID);
    GameEvent* Retreive();
    void Release();
};
