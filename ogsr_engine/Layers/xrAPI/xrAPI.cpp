// xrAPI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "../../Include/xrApi/xrAPI.h"

IRender_interface* Render = nullptr;
IRenderFactory* RenderFactory = nullptr;
CDUInterface* DU = nullptr;
xr_token* vid_mode_token = nullptr;
IUIRender* UIRender = nullptr;
CGameMtlLibrary* PGMLib = nullptr;
// #ifdef DEBUG
IDebugRender* DRender = nullptr;
// #endif // DEBUG
