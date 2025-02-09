#include "stdafx.h"

#ifdef DEBUG
BOOL bDebug = FALSE;
#endif

// Video
u32 psCurrentVidMode[2] = {0, 0};
// release version always has "mt_*" enabled
Flags32 psDeviceFlags = {rsDetails | rsDrawStatic | rsDrawDynamic | rsExclusiveMode};
