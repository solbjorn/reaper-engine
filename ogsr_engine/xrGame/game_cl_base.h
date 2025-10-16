#pragma once

#include "game_base.h"
#include "../xr_3da/NET_Server_Trash/client_id.h"
#include "WeaponAmmo.h"
// #include "Level_Bullet_Manager.h"

class NET_Packet;
class CGameObject;
class CUIGameCustom;
class CUI;
class CUIDialogWnd;

class game_cl_GameState : public game_GameState, public ISheduled
{
    RTTI_DECLARE_TYPEINFO(game_cl_GameState, game_GameState, ISheduled);

public:
    typedef game_GameState inherited;
    shared_str m_game_type_name;

protected:
    u16 m_u16VotingEnabled;
    bool m_bServerControlHits;

public:
    typedef xr_map<ClientID, game_PlayerState*> PLAYERS_MAP;
    typedef PLAYERS_MAP::iterator PLAYERS_MAP_IT;
    typedef PLAYERS_MAP::const_iterator PLAYERS_MAP_CIT;

    PLAYERS_MAP players;
    ClientID local_svdpnid;
    game_PlayerState* local_player{};
    bool m_need_to_update;

    virtual void reset_ui();

private:
    void switch_Phase(u32 new_phase) { inherited::switch_Phase(new_phase); }

protected:
    virtual void OnSwitchPhase(u32 old_phase, u32 new_phase);

    virtual shared_str shedule_Name() const { return shared_str("game_cl_GameState"); }
    virtual float shedule_Scale() const;
    virtual bool shedule_Needed() { return true; }

    void sv_EventSend(NET_Packet& P);

public:
    game_cl_GameState();
    virtual ~game_cl_GameState();

    LPCSTR type_name() const { return *m_game_type_name; }
    void set_type_name(LPCSTR s);
    virtual void Init() {}
    virtual void net_import_state(NET_Packet& P);
    virtual void net_import_update(NET_Packet& P);
    virtual void net_import_GameTime(NET_Packet& P); // update GameTime only for remote clients

    [[nodiscard]] bool IR_OnKeyboardPress(int dik);
    [[nodiscard]] bool IR_OnKeyboardRelease(int dik);
    [[nodiscard]] bool IR_OnMouseMove(int, int);
    [[nodiscard]] bool IR_OnMouseWheel(int);

    virtual bool OnKeyboardPress(int) { return false; }
    virtual bool OnKeyboardRelease(int) { return false; }

    game_PlayerState* GetPlayerByGameID(u32 GameID);
    game_PlayerState* GetPlayerByOrderID(u32 id);
    ClientID GetClientIDByOrderID(u32 id);
    u32 GetPlayersCount() const { return players.size(); }
    virtual CUIGameCustom* createGameUI() { return nullptr; }

    void StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators);
    virtual void shedule_Update(u32 dt);

    void u_EventGen(NET_Packet& P, u16 type, u16 dest);
    void u_EventSend(NET_Packet& P);

    virtual void OnRender() {}
    virtual bool IsServerControlHits() { return m_bServerControlHits; }
    virtual bool IsEnemy(game_PlayerState*) { return false; }
    virtual bool IsEnemy(CEntityAlive*, CEntityAlive*) { return false; }

    virtual void OnSpawn(CObject*) {}
    virtual void OnDestroy(CObject*) {}

    virtual void SendPickUpEvent(u16 ID_who, u16 ID_what);
};
