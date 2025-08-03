////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_type_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound type script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_sounds.h"
#include "script_sound_type.h"

template <>
void CScriptSoundType::script_register(sol::state_view& lua)
{
    lua.new_enum("snd_type", "no_sound", SOUND_TYPE_NO_SOUND, "weapon", SOUND_TYPE_WEAPON, "item", SOUND_TYPE_ITEM, "monster", SOUND_TYPE_MONSTER, "anomaly", SOUND_TYPE_ANOMALY,
                 "world", SOUND_TYPE_WORLD, "pick_up", SOUND_TYPE_PICKING_UP, "drop", SOUND_TYPE_DROPPING, "hide", SOUND_TYPE_HIDING, "take", SOUND_TYPE_TAKING, "use",
                 SOUND_TYPE_USING, "shoot", SOUND_TYPE_SHOOTING, "empty", SOUND_TYPE_EMPTY_CLICKING, "bullet_hit", SOUND_TYPE_BULLET_HIT, "reload", SOUND_TYPE_RECHARGING, "die",
                 SOUND_TYPE_DYING, "injure", SOUND_TYPE_INJURING, "step", SOUND_TYPE_STEP, "talk", SOUND_TYPE_TALKING, "attack", SOUND_TYPE_ATTACKING, "eat", SOUND_TYPE_EATING,
                 "idle", SOUND_TYPE_IDLE, "object_break", SOUND_TYPE_OBJECT_BREAKING, "object_collide", SOUND_TYPE_OBJECT_COLLIDING, "object_explode", SOUND_TYPE_OBJECT_EXPLODING,
                 "ambient", SOUND_TYPE_AMBIENT, "item_pick_up", SOUND_TYPE_ITEM_PICKING_UP, "item_drop", SOUND_TYPE_ITEM_DROPPING, "item_hide", SOUND_TYPE_ITEM_HIDING, "item_take",
                 SOUND_TYPE_ITEM_TAKING, "item_use", SOUND_TYPE_ITEM_USING, "weapon_shoot", SOUND_TYPE_WEAPON_SHOOTING, "weapon_empty", SOUND_TYPE_WEAPON_EMPTY_CLICKING,
                 "weapon_bullet_hit", SOUND_TYPE_WEAPON_BULLET_HIT, "weapon_reload", SOUND_TYPE_WEAPON_RECHARGING, "monster_die", SOUND_TYPE_MONSTER_DYING, "monster_injure",
                 SOUND_TYPE_MONSTER_INJURING, "monster_step", SOUND_TYPE_MONSTER_STEP, "monster_talk", SOUND_TYPE_MONSTER_TALKING, "monster_attack", SOUND_TYPE_MONSTER_ATTACKING,
                 "monster_eat", SOUND_TYPE_MONSTER_EATING, "anomaly_idle", SOUND_TYPE_ANOMALY_IDLE, "world_object_break", SOUND_TYPE_WORLD_OBJECT_BREAKING, "world_object_collide",
                 SOUND_TYPE_WORLD_OBJECT_COLLIDING, "world_object_explode", SOUND_TYPE_WORLD_OBJECT_EXPLODING, "world_ambient", SOUND_TYPE_WORLD_AMBIENT);
}
