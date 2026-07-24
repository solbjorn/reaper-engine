#ifndef KINEMATIC_ANIMATED_DEFS_INCLUDED
#define KINEMATIC_ANIMATED_DEFS_INCLUDED

#include "../../xr_3da/SkeletonMotionDefs.h"

// consts
constexpr inline u32 MAX_BLENDED{16};
constexpr inline u32 MAX_CHANNELS{4};

constexpr inline u32 MAX_BLENDED_POOL{MAX_BLENDED * MAX_PARTS * MAX_CHANNELS};
constexpr inline u32 MAX_ANIM_SLOT{48};

#endif // !KINEMATIC_ANIMATED_DEFS_INCLUDED
