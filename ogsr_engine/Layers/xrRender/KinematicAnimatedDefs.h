#ifndef KINEMATIC_ANIMATED_DEFS_INCLUDED
#define KINEMATIC_ANIMATED_DEFS_INCLUDED

#pragma once

#include "../../xr_3da/SkeletonMotionDefs.h"
// consts
constexpr u32 MAX_BLENDED = 16;
constexpr u32 MAX_CHANNELS = 4;

constexpr u32 MAX_BLENDED_POOL = (MAX_BLENDED * MAX_PARTS * MAX_CHANNELS);
constexpr u32 MAX_ANIM_SLOT = 48;
class CBlend;
typedef svector<CBlend*, MAX_BLENDED * MAX_CHANNELS> BlendSVec; //*MAX_CHANNELS
typedef BlendSVec::iterator BlendSVecIt;
typedef BlendSVec::const_iterator BlendSVecCIt;
//**********************************************************************************

#endif
