#pragma once

#include "../../xrCore/xrCore.h"

class IRender_interface;
extern IRender_interface* Render;

class IRenderFactory;
extern IRenderFactory* RenderFactory;

class CDUInterface;
extern CDUInterface* DU;

struct xr_token;
extern xr_token* vid_mode_token;

class IUIRender;
extern IUIRender* UIRender;

class CGameMtlLibrary;

class IDebugRender;
extern IDebugRender* DRender;
