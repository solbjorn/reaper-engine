#include "stdafx.h"

#include "eventapi.h"

#include "xr_ioconsole.h"

class CEvent
{
private:
    friend class CEventAPI;

    gsl::zstring Name;
    gsl::index dwRefCount{1};

    xr_vector<IEventReceiver*> Handlers;

public:
    explicit CEvent(gsl::czstring S)
    {
        Name = xr_strdup(S);
        _strupr(Name);
    }

    ~CEvent() { xr_free(Name); }

    void Attach(IEventReceiver* H)
    {
        if (std::find(Handlers.begin(), Handlers.end(), H) == Handlers.end())
            Handlers.push_back(H);
    }

    void Detach(const IEventReceiver* H)
    {
        auto I = std::find(Handlers.begin(), Handlers.end(), H);
        if (I != Handlers.end())
            Handlers.erase(I);
    }

    tmc::task<void> Signal(u64 P1, u64 P2)
    {
        for (auto handler : Handlers)
            co_await handler->OnEvent(this, P1, P2);
    }
};

void CEventAPI::Dump()
{
    std::ranges::sort(Events, [](const auto a, const auto b) { return std::is_lt(xr::strcasecmp(a->Name, b->Name)); });

    for (const auto event : Events)
        Msg("* [%zd] %s", event->dwRefCount, event->Name);
}

CEvent* CEventAPI::Create(gsl::czstring N)
{
    for (auto event : Events)
    {
        if (std::is_neq(xr::strcasecmp(event->Name, N)))
            continue;

        event->dwRefCount++;

        return event;
    }

    CEvent* X = xr_new<CEvent>(N);
    Events.push_back(X);

    return X;
}

void CEventAPI::Destroy(CEvent*& E)
{
    if (--E->dwRefCount != 0)
        return;

    auto I = std::find(Events.begin(), Events.end(), E);
    R_ASSERT(I != Events.end());

    Events.erase(I);
    xr_delete(E);
}

CEvent* CEventAPI::handler_attach_locked(gsl::czstring N, IEventReceiver* H)
{
    R_ASSERT(lock.is_locked());

    CEvent* E = Create(N);
    E->Attach(H);

    return E;
}

tmc::task<CEvent*> CEventAPI::Handler_Attach(gsl::czstring N, IEventReceiver* H)
{
    auto scope = co_await lock.lock_scope();
    co_return handler_attach_locked(N, H);
}

void CEventAPI::handler_detach_locked(CEvent*& E, const IEventReceiver* H)
{
    if (E == nullptr)
        return;

    R_ASSERT(lock.is_locked());

    E->Detach(H);
    Destroy(E);
}

tmc::task<void> CEventAPI::Handler_Detach(CEvent*& E, const IEventReceiver* H)
{
    if (E == nullptr)
        co_return;

    auto scope = co_await lock.lock_scope();
    handler_detach_locked(E, H);
}

#ifdef DEBUG
tmc::task<void> CEventAPI::Signal(gsl::czstring N, u64 P1, u64 P2)
{
    auto scope = co_await lock.lock_scope();

    CEvent* E = Create(N);
    co_await E->Signal(P1, P2);
    Destroy(E);
}
#endif

tmc::task<void> CEventAPI::Defer(gsl::czstring N, u64 P1, u64 P2)
{
    auto scope = co_await lock.lock_scope();

    CEvent* E = Create(N);
    E->dwRefCount++;

    Events_Deferred.emplace_back(E, P1, P2);
    Destroy(E);
}

tmc::task<void> CEventAPI::OnFrame()
{
    auto scope = co_await lock.lock_scope();

    if (Events_Deferred.empty())
        co_return;

    for (auto& event : Events_Deferred)
    {
        co_await event.E->Signal(event.P1, event.P2);
        Destroy(event.E);
    }

    Events_Deferred.clear();
}

bool CEventAPI::peek_locked(gsl::czstring EName)
{
    R_ASSERT(lock.is_locked());

    if (Events_Deferred.empty())
        return false;

    for (const auto& def : Events_Deferred)
    {
        if (std::is_eq(xr::strcasecmp(def.E->Name, EName)))
            return true;
    }

    return false;
}
