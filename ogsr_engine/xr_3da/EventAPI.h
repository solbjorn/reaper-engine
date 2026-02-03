#pragma once

#include "../xrCore/xrSyncronize.h"

class CEvent;

//---------------------------------------------------------------------

class XR_NOVTABLE IEventReceiver : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IEventReceiver);

public:
    ~IEventReceiver() override = 0;

    virtual tmc::task<void> OnEvent(CEvent* E, u64 P1, u64 P2) = 0;
};

inline IEventReceiver::~IEventReceiver() = default;

//---------------------------------------------------------------------

class CEventAPI
{
private:
    struct Deferred
    {
        CEvent* E;
        u64 P1;
        u64 P2;
    };

    xr_vector<CEvent*> Events;
    xr_vector<Deferred> Events_Deferred;
    tmc::mutex lock;

    [[nodiscard]] CEvent* Create(gsl::czstring N);
    void Destroy(CEvent*& E);

public:
    [[nodiscard]] CEvent* handler_attach_locked(gsl::czstring N, IEventReceiver* H);
    tmc::task<CEvent*> Handler_Attach(gsl::czstring N, IEventReceiver* H);

    void handler_detach_locked(CEvent*& E, const IEventReceiver* H);
    tmc::task<void> Handler_Detach(CEvent*& E, const IEventReceiver* H);

#ifdef DEBUG
    tmc::task<void> Signal(gsl::czstring E, u64 P1 = 0, u64 P2 = 0);
#endif

    tmc::task<void> Defer(gsl::czstring E, u64 P1 = 0, u64 P2 = 0);

    tmc::task<void> OnFrame();
    void Dump();
    [[nodiscard]] bool peek_locked(gsl::czstring EName);
};
