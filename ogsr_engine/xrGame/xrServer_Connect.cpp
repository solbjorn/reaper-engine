#include "stdafx.h"

#include "xrServer.h"

#include "MainMenu.h"
#include "game_cl_single.h"
#include "game_sv_single.h"
#include "xrMessages.h"

tmc::task<xrServer::EConnect> xrServer::Connect(shared_str& session_name)
{
#ifdef DEBUG
    Msg("* sv_Connect: {}", session_name);
#endif

    // Parse options and create game
    if (!std::string_view{session_name}.contains('/'))
        co_return ErrConnect;

    game = nullptr;

    CLASS_ID clsid = game_GameState::getCLASS_ID(true);
    game = smart_cast<game_sv_GameState*>(NEW_INSTANCE(clsid));

    // Options
    if (!game)
        co_return ErrConnect;

    csPlayers.Enter();

#ifdef DEBUG
    Msg("* Created server_game {}", game->type_name());
#endif

    co_await game->Create(session_name);
    csPlayers.Leave();

    co_return co_await IPureServer::Connect(session_name);
}

IClient* xrServer::new_client(SClientConnectData* cl_data)
{
    IClient* CL = client_Find_Get(cl_data->clientID);
    VERIFY(CL);

    // copy entity
    CL->ID = cl_data->clientID;
    CL->process_id = cl_data->process_id;

    string64 new_name;
    strcpy_s(new_name, cl_data->name);
    CL->name._set(new_name);
    CL->pass._set(cl_data->pass);

    NET_Packet P;
    P.B.count = 0;
    P.r_pos = 0;

    game->AddDelayedEvent(P, GAME_EVENT_CREATE_CLIENT, 0, CL->ID);
    if (client_Count() == 1)
    {
        Update();
    }
    return CL;
}

void xrServer::AttachNewClient(IClient* CL)
{
    MSYS_CONFIG msgConfig;
    msgConfig.sign1 = 0x12071980;
    msgConfig.sign2 = 0x26111975;

    SV_Client = CL;
    CL->flags.bLocal = 1;
    SendTo_LL(SV_Client->ID, &msgConfig, sizeof(msgConfig), net_flags(TRUE, TRUE, TRUE, TRUE));

    CL->m_guid[0] = 0;
}
