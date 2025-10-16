#pragma once

#include "net_shared.h"

struct ip_address;

class INetQueue
{
    xrCriticalSection cs;
    xr_deque<NET_Packet*> ready;
    xr_vector<NET_Packet*> unused;

public:
    INetQueue();
    ~INetQueue();

    NET_Packet* CreateGet();
    void CreateCommit(NET_Packet*);

    NET_Packet* Retreive();
    void Release();
};

class XR_NOVTABLE IPureClient : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureClient);

public:
    enum ConnectionState
    {
        EnmConnectionFails = 0,
        EnmConnectionWait = -1,
        EnmConnectionCompleted = 1
    };

protected:
    CTimer* device_timer;

    ConnectionState net_Connected{};

    INetQueue net_Queue;

public:
    IPureClient(CTimer* tm);
    virtual ~IPureClient();

    BOOL Connect(LPCSTR server_name);
    void Disconnect();

    BOOL net_isFails_Connect() { return net_Connected == EnmConnectionFails; }
    LPCSTR net_SessionName() { return nullptr; }

    // receive
    IC virtual NET_Packet* net_msg_Retreive() { return net_Queue.Retreive(); }
    IC void net_msg_Release() { net_Queue.Release(); }

    // send
    virtual void Send(NET_Packet&, u32 = DPNSEND_GUARANTEED, u32 = 0);
    virtual void OnMessage(void* data, u32 size);
    virtual void OnConnectRejected() {}

    // time management
    IC u32 timeServer() { return device_timer->GetElapsed_ms(); }
    IC u32 timeServer_Async() { return device_timer->GetElapsed_ms(); }

    BOOL net_Syncronised{};
    virtual BOOL net_IsSyncronised() { return net_Syncronised; }

    virtual void OnSessionTerminate(LPCSTR) {}
};
