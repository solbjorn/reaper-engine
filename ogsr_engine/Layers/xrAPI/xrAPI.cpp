// xrAPI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "../../Include/xrAPI/xrAPI.h"

IRender_interface* Render = nullptr;
IRenderFactory* RenderFactory = nullptr;
CDUInterface* DU = nullptr;
xr_token* vid_mode_token = nullptr;
IUIRender* UIRender = nullptr;
IDebugRender* DRender = nullptr;
