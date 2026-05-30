#include "../stdafx.h"

#include "net_server.h"

ClientID BroadcastCID(0xffffffff);

void ip_address::set(LPCSTR src_string) // Это нужно
{
    const auto res = scn::scan<u8, u8, u8, u8>(std::string_view{src_string}, "{}.{}.{}.{}");
    if (res)
    {
        const auto [a1, a2, a3, a4] = res->values();
        m_data.a1 = a1;
        m_data.a2 = a2;
        m_data.a3 = a3;
        m_data.a4 = a4;
    }
    else
    {
        Msg("! Bad ipAddress format [{}]: {}", src_string, res.error().msg());
        m_data.data = 0;
    }
}

IClient::IClient()
{
    dwTime_LastUpdate = 0;
    flags.bLocal = FALSE;
    flags.bConnected = FALSE;
    flags.bReconnect = FALSE;
    flags.bVerified = TRUE;
}

IClient::~IClient() {}

IClient* IPureServer::ID_to_client(ClientID ID, bool ScanAll) // пока не резать. net_Players - не пустой вектор
{
    if (!ID.value())
        return nullptr;

    csPlayers.Enter();

    for (u32 client = 0; client < net_Players.size(); ++client)
    {
        if (net_Players[client]->ID == ID)
        {
            csPlayers.Leave();
            return net_Players[client];
        }
    }

    if (ScanAll)
    {
        for (u32 client = 0; client < net_Players_disconnected.size(); ++client)
        {
            if (net_Players_disconnected[client]->ID == ID)
            {
                csPlayers.Leave();
                return net_Players_disconnected[client];
            }
        }
    }

    csPlayers.Leave();

    return nullptr;
}

IPureServer::IPureServer(CTimer* timer) : device_timer{timer} {}
IPureServer::~IPureServer() { SV_Client = nullptr; }

tmc::task<IPureServer::EConnect> IPureServer::Connect(shared_str& options) // опции вида [имя_сейва/single/alife]
{
    connect_options = options;
    co_return ErrNoError;
}

void IPureServer::Disconnect() {}

void IPureServer::SendTo_LL(ClientID, void*, u32, u32, u32) { FATAL(""); }

void IPureServer::SendTo(ClientID ID /*DPNID ID*/, NET_Packet& P, u32 dwFlags, u32 dwTimeout) // Отсюда отправляются данные в IPureClient::OnMessage
{
    SendTo_LL(ID, P.B.data, P.B.count, dwFlags, dwTimeout);
}

void IPureServer::SendBroadcast_LL(ClientID exclude, void* data, u32 size, u32 dwFlags) // Отсюда отправляются данные в IPureClient::OnMessage
{
    csPlayers.Enter();

    for (u32 i = 0; i < net_Players.size(); i++)
    {
        IClient* player = net_Players[i];

        if (player->ID == exclude)
            continue;
        if (!player->flags.bConnected)
            continue;

        SendTo_LL(player->ID, data, size, dwFlags);
    }

    csPlayers.Leave();
}

void IPureServer::SendBroadcast(ClientID exclude, NET_Packet& P, u32 dwFlags)
{
    // Perform broadcasting
    SendBroadcast_LL(exclude, P.B.data, P.B.count, dwFlags);
}

u32 IPureServer::OnMessage(NET_Packet&, ClientID) { return 0; }
