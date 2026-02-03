// EngineAPI.cpp: implementation of the CEngineAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "EngineAPI.h"

#include "xr_ioconsole.h"
#include "xr_ioc_cmd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineAPI::CEngineAPI() = default;
CEngineAPI::~CEngineAPI() = default;

void CEngineAPI::Initialize()
{
    AttachRender();

    Console->Execute("renderer renderer_r4");
    Device.ConnectToRender();

    pCreate = &xrFactory_Create;
    pDestroy = &xrFactory_Destroy;

    AttachGame();
}

void CEngineAPI::Destroy()
{
    pCreate = nullptr;
    pDestroy = nullptr;

    Engine.Event.Dump();
    XRC.r_clear_compact();

    // destroy quality token here
    if (vid_quality_token)
    {
        for (int i = 0; vid_quality_token[i].name; i++)
        {
            gsl::zstring name = const_cast<gsl::zstring>(vid_quality_token[i].name);
            xr_free(name);
        }

        xr_free(vid_quality_token);
        vid_quality_token = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEngineAPI::CreateRendererList()
{
    vid_quality_token = xr_alloc<xr_token>(2);

    vid_quality_token[1].id = -1;
    vid_quality_token[1].name = nullptr;

    vid_quality_token[0].id = 0;
    vid_quality_token[0].name = xr_strdup("renderer_r4");
}
