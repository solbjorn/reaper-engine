#pragma once

#include "game_base.h"
#include "alife_space.h"
#include "script_export_space.h"
#include "../xr_3da/NET_Server_Trash/client_id.h"

class CSE_Abstract;
class CUISequencer;
class GameEventQueue;
class xrServer;

class XR_NOVTABLE game_sv_GameState : public game_GameState
{
    RTTI_DECLARE_TYPEINFO(game_sv_GameState, game_GameState);

public:
    typedef game_GameState inherited;

protected:
    //	u32								m_RPointFreezeTime;
    xrServer* m_server;
    GameEventQueue* m_event_queue;
    //	BOOL							m_bVotingEnabled;

    // Events
    virtual void OnEvent(NET_Packet& tNetPacket, u16 type, u32, ClientID sender);

public:
    BOOL sv_force_sync{};

public:
    virtual void OnPlayerConnect(ClientID);

public:
    game_sv_GameState();
    ~game_sv_GameState() override;

    // Main accessors
    virtual void* get_client(u16 id); // if exist
    virtual game_PlayerState* get_it(u32 it);
    virtual game_PlayerState* get_id(ClientID id);

    virtual u16 get_id_2_eid(ClientID id);
    virtual ClientID get_it_2_id(u32 it);

    virtual u32 get_players_count();
    CSE_Abstract* get_entity_from_eid(u16 id);

    // Signals
    virtual void signal_Syncronize();

    virtual void OnSwitchPhase(u32 old_phase, u32 new_phase);

    // Utilities
    void u_EventGen(NET_Packet& P, u16 type, u16 dest);
    void u_EventSend(NET_Packet& P, u32 dwFlags = 0x0008);

    // Events
    virtual BOOL OnPreCreate(CSE_Abstract*) { return TRUE; }
    virtual void OnCreate(u16) {}
    virtual void OnPostCreate(u16) {}
    virtual BOOL OnTouch(u16 eid_who, u16 eid_target, BOOL bForced = FALSE) = 0; // TRUE=allow ownership, FALSE=denied
    virtual void OnDetach(u16 eid_who, u16 eid_target) = 0;

    // Main
    virtual void Create(shared_str&);
    virtual void Update();

    virtual void net_Export_State(NET_Packet& P, ClientID id_to); // full state
    virtual void net_Export_Update(NET_Packet& P, ClientID id_to, ClientID id); // just incremental update for specific client
    virtual void net_Export_GameTime(NET_Packet& P); // update GameTime only for remote clients

    [[nodiscard]] virtual bool change_level(NET_Packet&, ClientID);
    virtual void save_game(NET_Packet&, ClientID);
    [[nodiscard]] virtual bool load_game(NET_Packet&, ClientID);
    virtual void switch_distance(NET_Packet&, ClientID);

    void AddDelayedEvent(NET_Packet& tNetPacket, u16 type, u32 time, ClientID sender);
    void ProcessDelayedEvent();

    virtual void teleport_object(NET_Packet&, u16);

    virtual void add_restriction(NET_Packet&, u16);
    virtual void remove_restriction(NET_Packet&, u16);
    virtual void remove_all_restrictions(NET_Packet&, u16);

    virtual bool custom_sls_default() { return false; }
    virtual void sls_default() {}

    virtual shared_str level_name(const shared_str& server_options) const;
    virtual void on_death(CSE_Abstract* e_dest, CSE_Abstract* e_src);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(game_sv_GameState);
#undef script_type_list
#define script_type_list save_type_list(game_sv_GameState)

// game_sv_base_script.cpp
extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

void start_tutorial(LPCSTR name);
