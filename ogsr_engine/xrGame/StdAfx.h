#pragma once

#include "../xr_3da/stdafx.h"
#include "../xrScriptEngine/xr_sol.h"
#include "..\xr_3da\ai_script_space.h" //KRodin: пусть луа для рендера и скриптов инитится в одном месте.

#define THROW VERIFY
#define THROW2 VERIFY2
#define THROW3 VERIFY3

#include "../xr_3da/gamefont.h"
#include "../xr_3da/xr_object.h"
#include "../xr_3da/igame_level.h"

#include "../xrCore/_vector3d_ext.h"

#define D3DCOLOR_RGBA(r, g, b, a) (color_argb(a, r, g, b))
#define D3DCOLOR_XRGB(r, g, b) (color_argb(0xff, r, g, b))
