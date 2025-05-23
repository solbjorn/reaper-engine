#include "stdafx.h"
#include "level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "game_cl_base.h"
#include "xrmessages.h"
#include "../xr_3da/x_ray.h"
#include "../xr_3da/device.h"
#include "..\xr_3da\IGame_Persistent.h"
#include "..\xr_3da\XR_IOConsole.h"
#include "MainMenu.h"

BOOL CLevel::net_Start(LPCSTR op_server, LPCSTR op_client)
{
    net_start_result_total = TRUE;

    pApp->LoadBegin();

    // make Client Name if options doesn't have it
    LPCSTR NameStart = strstr(op_client, "/name=");
    if (!NameStart)
    {
        string512 tmp;
        strcpy_s(tmp, op_client);
        strcat_s(tmp, "/name=");
        strcat_s(tmp, xr_strlen(Core.UserName) ? Core.UserName : Core.CompName);
        m_caClientOptions = tmp;
    }
    else
    {
        string1024 ret = "";
        LPCSTR begin = NameStart + xr_strlen("/name=");
        sscanf(begin, "%[^/]", ret);
        if (!xr_strlen(ret))
        {
            string1024 tmpstr;
            strcpy_s(tmpstr, op_client);
            *(strstr(tmpstr, "name=") + 5) = 0;
            strcat_s(tmpstr, xr_strlen(Core.UserName) ? Core.UserName : Core.CompName);
            const char* ptmp = strchr(strstr(op_client, "name="), '/');
            if (ptmp)
                strcat_s(tmpstr, ptmp);
            m_caClientOptions = tmpstr;
        }
        else
        {
            m_caClientOptions = op_client;
        };
    };
    m_caServerOptions = op_server;
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------------
    g_loading_events.push_back(CallMe::fromMethod<&CLevel::net_start1>(this));
    g_loading_events.push_back(CallMe::fromMethod<&CLevel::net_start2>(this));
    g_loading_events.push_back(CallMe::fromMethod<&CLevel::net_start3>(this));
    g_loading_events.push_back(CallMe::fromMethod<&CLevel::net_start4>(this));
    g_loading_events.push_back(CallMe::fromMethod<&CLevel::net_start5>(this));
    g_loading_events.push_back(CallMe::fromMethod<&CLevel::net_start6>(this));

    return net_start_result_total;
}

bool CLevel::net_start1()
{
    // Start client and server if need it
    if (m_caServerOptions.size())
    {
        g_pGamePersistent->LoadTitle("st_server_starting");

        typedef IGame_Persistent::params params;
        params& p = g_pGamePersistent->m_game_params;
        // Connect
        Server = xr_new<xrServer>();

        //		if (!strstr(*m_caServerOptions,"/alife"))
        if (xr_strcmp(p.m_alife, "alife"))
        {
            string64 l_name = "";
            const char* SOpts = *m_caServerOptions;
            strncpy_s(l_name, *m_caServerOptions, strchr(SOpts, '/') - SOpts);
            // Activate level
            if (strchr(l_name, '/'))
                *strchr(l_name, '/') = 0;

            m_name = l_name;

            int id = pApp->Level_ID(l_name);

            if (id < 0)
            {
                pApp->LoadEnd();
                Msg("Can't find level: [%s]", l_name);
                net_start_result_total = FALSE;
                return true;
            }
        }
    }
    return true;
}

bool CLevel::net_start2()
{
    if (net_start_result_total && m_caServerOptions.size())
    {
        if ((m_connect_server_err = Server->Connect(m_caServerOptions)) != xrServer::ErrNoError)
        {
            net_start_result_total = false;
            Msg("! Failed to start server.");
            //			Console->Execute("main_menu on");
            return true;
        }
        Server->SLS_Default();
        m_name = Server->level_name(m_caServerOptions);
    }
    return true;
}

bool CLevel::net_start3()
{
    if (!net_start_result_total)
        return true;

    return true;
}

bool CLevel::net_start4()
{
    if (!net_start_result_total)
        return true;

    g_loading_events.pop_front();

    g_loading_events.push_front(CallMe::fromMethod<&CLevel::net_start_client6>(this));
    g_loading_events.push_front(CallMe::fromMethod<&CLevel::net_start_client5>(this));
    g_loading_events.push_front(CallMe::fromMethod<&CLevel::net_start_client4>(this));
    g_loading_events.push_front(CallMe::fromMethod<&CLevel::net_start_client3>(this));
    g_loading_events.push_front(CallMe::fromMethod<&CLevel::net_start_client2>(this));
    g_loading_events.push_front(CallMe::fromMethod<&CLevel::net_start_client1>(this));

    return false;
}

bool CLevel::net_start5()
{
    if (net_start_result_total)
    {
        NET_Packet NP;
        NP.w_begin(M_CLIENTREADY);
        Send(NP, net_flags(TRUE, TRUE));
    };
    return true;
}
#include "hudmanager.h"
BOOL g_start_total_res = TRUE;
xrServer::EConnect g_connect_server_err = xrServer::ErrConnect;

struct LevelLoadFinalizer
{
    bool net_start_finalizer() { return true; }
};
LevelLoadFinalizer LF;

bool CLevel::net_start6()
{
    g_start_total_res = net_start_result_total;
    g_connect_server_err = m_connect_server_err;
    g_loading_events.pop_front();
    g_loading_events.push_front(CallMe::fromMethod<&LevelLoadFinalizer::net_start_finalizer>(&LF));

    // init bullet manager
    BulletManager().Clear();
    BulletManager().Load();

    pApp->LoadEnd();

    if (net_start_result_total)
    {
        if (strstr(Core.Params, "-$"))
        {
            string256 buf, cmd, param;
            sscanf(strstr(Core.Params, "-$") + 2, "%[^ ] %[^ ] ", cmd, param);
            strconcat(sizeof(buf), buf, cmd, " ", param);
            Console->Execute(buf);
        }

        if (g_hud)
            HUD().GetUI()->OnConnected();
    }

    return false;
}

void CLevel::InitializeClientGame(NET_Packet& P)
{
    string256 game_type_name;
    P.r_stringZ(game_type_name);
    if (game && !xr_strcmp(game_type_name, game->type_name()))
        return;

    xr_delete(game);
    Msg("- Game configuring : Started ");
    CLASS_ID clsid = game_GameState::getCLASS_ID(game_type_name, false);
    game = smart_cast<game_cl_GameState*>(NEW_INSTANCE(clsid));
    game->set_type_name(game_type_name);
    game->Init();
    m_bGameConfigStarted = TRUE;

    R_ASSERT(Load_GameSpecific_After());
}
