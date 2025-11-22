#ifndef __XRGAME_MT_CONFIG_H
#define __XRGAME_MT_CONFIG_H

////////////////////////////////////////////////////////////////////////////
//	Module 		: mt_config.h
//	Created 	: 22.02.2005
//  Modified 	: 22.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Multithreading configuration options
////////////////////////////////////////////////////////////////////////////

#define mtLevelPath (1 << 0)
#define mtDetailPath (1 << 1)
#define mtObjectHandler (1 << 2)
#define mtSoundPlayer (1 << 3)
#define mtAiVision (1 << 4)
#define mtBullets (1 << 5)
#define mtLUA_GC (1 << 6)
#define mtLevelSounds (1 << 7)
#define mtALife (1 << 8)
#define mtMap (1 << 9)

constexpr inline Flags32 g_mt_default{mtLevelPath | mtDetailPath | mtObjectHandler | mtSoundPlayer | mtAiVision | mtBullets | mtLUA_GC | mtLevelSounds | mtALife | mtMap};

#ifdef MASTER_GOLD
constexpr inline Flags32 g_mt_config{g_mt_default};

#define XR_RELEASE_CONSTEXPR constexpr
#else
extern Flags32 g_mt_config;

#define XR_RELEASE_CONSTEXPR
#endif

#endif // __XRGAME_MT_CONFIG_H
