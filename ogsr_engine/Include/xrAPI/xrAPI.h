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
extern CGameMtlLibrary* PGMLib;

// #ifdef DEBUG
class IDebugRender;
extern IDebugRender* DRender;
// #endif // DEBUG
