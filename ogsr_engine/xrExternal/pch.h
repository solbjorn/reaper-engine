#ifndef __XREXTERNAL_PCH_H
#define __XREXTERNAL_PCH_H

#include "../xrCore/defines.h"

#define DISCORDPP_IMPLEMENTATION
#define TMC_IMPL
#define XR_TRACY_BUILDING

#include "imgui.h"
#include "rtti.h"
#include "sol.h"
#include "tmc.h"
#include "tracy.h"

#ifndef DISCORDPP_IMPLEMENTATION
#include <discordpp.h>
#endif

#endif // !__XREXTERNAL_PCH_H
