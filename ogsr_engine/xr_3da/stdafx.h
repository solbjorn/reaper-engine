#pragma once

#include "../xrCore/xrCore.h"
#include "../Include/xrAPI/xrAPI.h"

// Our headers
#include "context.h"
#include "Engine.h"
#include "defines.h"
#include "device.h"
#include "FS.h"

#include "../xrCDB/xrXRC.h"

#include "../xrSound/Sound.h"

extern CInifile* pGameIni;

#define READ_IF_EXISTS(ltx, method, section, name, default_value) (((ltx)->line_exist(section, name)) ? ((ltx)->method(section, name)) : (default_value))
