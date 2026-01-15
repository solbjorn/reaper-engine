#pragma once

#include "../xrCore/xrSyncronize.h"

class CEvent;
typedef CEvent* EVENT;

//---------------------------------------------------------------------

class XR_NOVTABLE IEventReceiver : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IEventReceiver);

public:
    ~IEventReceiver() override = 0;

    virtual tmc::task<void> OnEvent(EVENT E, u64 P1, u64 P2) = 0;
};

inline IEventReceiver::~IEventReceiver() = default;

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
    mutable xrCriticalSection CS;

public:
    CEventAPI() = default;

    EVENT Create(const char* N);
    void Destroy(EVENT& E);

    EVENT Handler_Attach(const char* N, IEventReceiver* H);
    void Handler_Detach(EVENT& E, IEventReceiver* H);

    tmc::task<void> Signal(EVENT E, u64 P1 = 0, u64 P2 = 0);
    tmc::task<void> Signal(gsl::czstring E, u64 P1 = 0, u64 P2 = 0);
    void Defer(EVENT E, u64 P1 = 0, u64 P2 = 0);
    void Defer(LPCSTR E, u64 P1 = 0, u64 P2 = 0);

    tmc::task<void> OnFrame();
    void Dump();
    [[nodiscard]] bool Peek(gsl::czstring EName) const;

    void _destroy();
};
