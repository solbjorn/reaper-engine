#pragma once

#include "../xr_3da/stdafx.h"
#include "../xrExternal/sol.h"

#include "../xr_3da/GameFont.h"
#include "../xr_3da/xr_object.h"
#include "../xr_3da/IGame_Level.h"

#include "../xrCore/_vector3d_ext.h"

#define D3DCOLOR_RGBA(r, g, b, a) (color_argb(a, r, g, b))
#define D3DCOLOR_XRGB(r, g, b) (color_argb(0xff, r, g, b))
