#include "stdafx.h"

#include "xrSheduler.h"
#include "xr_object.h"

ISheduled::ISheduled()
{
    shedule.t_min = 20;
    shedule.t_max = 1000;
    shedule.b_locked = FALSE;

#ifdef DEBUG
    dbg_startframe = 1;
    dbg_update_shedule = 0;
#endif
}

ISheduled::~ISheduled()
{
    XR_DEBUG_ASSERT(!Engine.Sheduler.Registered(this), "", shedule_Name());

#ifndef DEBUG
    // sad, but true
    // we need this to become MASTER_GOLD
    Engine.Sheduler.Unregister(this);
#endif // DEBUG
}

void ISheduled::shedule_register() { Engine.Sheduler.Register(this); }
void ISheduled::shedule_unregister(bool force) { Engine.Sheduler.Unregister(this, force); }

#ifdef DEBUG
tmc::task<void> ISheduled::shedule_Update(u32)
{
    if (dbg_startframe == dbg_update_shedule)
    {
        std::string_view name{"unknown"};

        if (auto O = smart_cast<CObject*>(this); O != nullptr)
            name = O->cName();

        XR_PANIC("object update called twice per frame", name, dbg_startframe);
    }

    dbg_update_shedule = dbg_startframe;
    co_return;
}
#endif
