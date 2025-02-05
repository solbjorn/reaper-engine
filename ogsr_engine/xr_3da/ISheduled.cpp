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
    VERIFY2(!Engine.Sheduler.Registered(this), *shedule_Name());

#ifndef DEBUG
    // sad, but true
    // we need this to become MASTER_GOLD
    Engine.Sheduler.Unregister(this);
#endif // DEBUG
}

void ISheduled::shedule_register() { Engine.Sheduler.Register(this); }
void ISheduled::shedule_unregister(bool force) { Engine.Sheduler.Unregister(this, force); }

#ifdef DEBUG
void ISheduled::shedule_Update(u32)
{
    if (dbg_startframe == dbg_update_shedule)
    {
        LPCSTR name = "unknown";
        CObject* O = smart_cast<CObject*>(this);
        if (O)
            name = *O->cName();
        Debug.fatal(DEBUG_INFO, "'shedule_Update' called twice per frame for %s", name);
    }

    dbg_update_shedule = dbg_startframe;
}
#endif
