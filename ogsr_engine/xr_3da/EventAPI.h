#pragma once

#include "../xrCore/xrSyncronize.h"

class CEvent;
typedef CEvent* EVENT;

//---------------------------------------------------------------------
class IEventReceiver
{
public:
    virtual void OnEvent(EVENT E, u64 P1, u64 P2) = 0;
};
//---------------------------------------------------------------------
class CEventAPI
{
    struct Deferred
    {
        EVENT E;
        u64 P1;
        u64 P2;
    };

private:
    xr_vector<EVENT> Events;
    xr_vector<Deferred> Events_Deferred;
    xrCriticalSection CS;

public:
#ifdef PROFILE_CRITICAL_SECTIONS
    CEventAPI() : CS(MUTEX_PROFILE_ID(CEventAPI)) {}
#endif // PROFILE_CRITICAL_SECTIONS
    EVENT Create(const char* N);
    void Destroy(EVENT& E);

    EVENT Handler_Attach(const char* N, IEventReceiver* H);
    void Handler_Detach(EVENT& E, IEventReceiver* H);

    void Signal(EVENT E, u64 P1 = 0, u64 P2 = 0);
    void Signal(LPCSTR E, u64 P1 = 0, u64 P2 = 0);
    void Defer(EVENT E, u64 P1 = 0, u64 P2 = 0);
    void Defer(LPCSTR E, u64 P1 = 0, u64 P2 = 0);

    void OnFrame();
    void Dump();
    BOOL Peek(LPCSTR EName);

    void _destroy();
};
