#include "stdafx.h"

#include "eventapi.h"

#include "xr_ioconsole.h"

class CEvent
{
    friend class CEventAPI;

private:
    char* Name;
    xr_vector<IEventReceiver*> Handlers;
    u32 dwRefCount;

public:
    explicit CEvent(gsl::czstring S);
    ~CEvent();

    LPCSTR GetFull() { return Name; }
    u32 RefCount() { return dwRefCount; }

    [[nodiscard]] bool Equal(const CEvent& E) const { return std::is_eq(xr::strcasecmp(Name, E.Name)); }

    void Attach(IEventReceiver* H)
    {
        if (std::find(Handlers.begin(), Handlers.end(), H) == Handlers.end())
            Handlers.push_back(H);
    }
    void Detach(IEventReceiver* H)
    {
        xr_vector<IEventReceiver*>::iterator I = std::find(Handlers.begin(), Handlers.end(), H);
        if (I != Handlers.end())
            Handlers.erase(I);
    }

    [[nodiscard]] tmc::task<void> Signal(u64 P1, u64 P2)
    {
        for (auto handler : Handlers)
            co_await handler->OnEvent(this, P1, P2);
    }
};

CEvent::CEvent(const char* S)
{
    Name = xr_strdup(S);
    _strupr(Name);
    dwRefCount = 1;
}
CEvent::~CEvent() { xr_free(Name); }

void CEventAPI::Dump()
{
    std::ranges::sort(Events, [](CEvent* E1, CEvent* E2) { return E1->GetFull() < E2->GetFull(); });
    for (u32 i = 0; i < Events.size(); i++)
        Msg("* [%u32] %s", Events[i]->RefCount(), Events[i]->GetFull());
}

EVENT CEventAPI::Create(const char* N)
{
    CS.Enter();
    CEvent E(N);
    for (xr_vector<CEvent*>::iterator I = Events.begin(); I != Events.end(); I++)
    {
        if ((*I)->Equal(E))
        {
            EVENT F = *I;
            F->dwRefCount++;
            CS.Leave();
            return F;
        }
    }

    EVENT X = xr_new<CEvent>(N);
    Events.push_back(X);
    CS.Leave();
    return X;
}
void CEventAPI::Destroy(EVENT& E)
{
    CS.Enter();
    E->dwRefCount--;
    if (E->dwRefCount == 0)
    {
        xr_vector<CEvent*>::iterator I = std::find(Events.begin(), Events.end(), E);
        R_ASSERT(I != Events.end());
        Events.erase(I);
        xr_delete(E);
    }
    CS.Leave();
}

EVENT CEventAPI::Handler_Attach(const char* N, IEventReceiver* H)
{
    CS.Enter();
    EVENT E = Create(N);
    E->Attach(H);
    CS.Leave();
    return E;
}

void CEventAPI::Handler_Detach(EVENT& E, IEventReceiver* H)
{
    if (!E)
        return;

    CS.Enter();
    E->Detach(H);
    Destroy(E);
    CS.Leave();
}

tmc::task<void> CEventAPI::Signal(EVENT E, u64 P1, u64 P2)
{
    CS.Enter();
    co_await E->Signal(P1, P2);
    CS.Leave();
}

tmc::task<void> CEventAPI::Signal(gsl::czstring N, u64 P1, u64 P2)
{
    CS.Enter();
    EVENT E = Create(N);
    co_await Signal(E, P1, P2);
    Destroy(E);
    CS.Leave();
}

void CEventAPI::Defer(EVENT E, u64 P1, u64 P2)
{
    CS.Enter();
    E->dwRefCount++;
    Events_Deferred.emplace_back();
    Events_Deferred.back().E = E;
    Events_Deferred.back().P1 = P1;
    Events_Deferred.back().P2 = P2;
    CS.Leave();
}

void CEventAPI::Defer(LPCSTR N, u64 P1, u64 P2)
{
    CS.Enter();
    EVENT E = Create(N);
    Defer(E, P1, P2);
    Destroy(E);
    CS.Leave();
}

tmc::task<void> CEventAPI::OnFrame()
{
    CS.Enter();

    if (Events_Deferred.empty())
    {
        CS.Leave();
        co_return;
    }

    for (auto& event : Events_Deferred)
    {
        co_await Signal(event.E, event.P1, event.P2);
        Destroy(event.E);
    }

    Events_Deferred.clear();
    CS.Leave();
}

bool CEventAPI::Peek(gsl::czstring EName) const
{
    CS.Enter();

    if (Events_Deferred.empty())
    {
        CS.Leave();
        return false;
    }

    for (const auto& def : Events_Deferred)
    {
        if (std::is_eq(xr::strcasecmp(def.E->GetFull(), EName)))
        {
            CS.Leave();
            return true;
        }
    }

    CS.Leave();

    return false;
}

void CEventAPI::_destroy()
{
    Dump();
    if (Events.empty())
        Events.clear();
    if (Events_Deferred.empty())
        Events_Deferred.clear();
}
