////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGame.cpp
//	Created 	: 07.01.2001
//  Modified 	: 27.05.2004
//	Author		: Aleksandr Maksimchuk and Oles' Shyshkovtsov
//	Description : Defines the entry point for the DLL application.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GamePersistent.h"
#include "game_base.h"
#include "object_factory.h"
#include "ui/xrUIXmlParser.h"
#include "xr_level_controller.h"
#include "profiler.h"

DLL_Pure* xrFactory_Create(CLASS_ID clsid)
{
    DLL_Pure* object = object_factory().client_object(clsid);
#ifdef DEBUG
    if (object == nullptr)
        return nullptr;
#endif

    object->CLS_ID = clsid;

    return object;
}

void xrFactory_Destroy(DLL_Pure* O) { xr_delete(O); }

extern shared_str g_active_task_id;

void AttachGame()
{
    g_fTimeFactor = pSettings->r_float("alife", "time_factor");
    g_active_task_id._set("");

    // register console commands
    CCC_RegisterCommands();
    // keyboard binding
    CCC_RegisterInput();

#ifdef DEBUG
    g_profiler = xr_new<CProfiler>();
#endif
}
