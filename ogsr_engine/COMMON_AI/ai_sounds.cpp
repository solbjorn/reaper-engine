#include "stdafx.h"

#include "ai_sounds.h"

const xr_token anomaly_type_token[] = {{"undefined", static_cast<long>(sg_Undefined)},
                                       {"Item picking up", static_cast<long>(SOUND_TYPE_ITEM_PICKING_UP)},
                                       {"Item dropping", static_cast<long>(SOUND_TYPE_ITEM_DROPPING)},
                                       {"Item taking", static_cast<long>(SOUND_TYPE_ITEM_TAKING)},
                                       {"Item hiding", static_cast<long>(SOUND_TYPE_ITEM_HIDING)},
                                       {"Item using", static_cast<long>(SOUND_TYPE_ITEM_USING)},
                                       {"Weapon shooting", static_cast<long>(SOUND_TYPE_WEAPON_SHOOTING)},
                                       {"Weapon empty clicking", static_cast<long>(SOUND_TYPE_WEAPON_EMPTY_CLICKING)},
                                       {"Weapon bullet hit", static_cast<long>(SOUND_TYPE_WEAPON_BULLET_HIT)},
                                       {"Weapon recharging", static_cast<long>(SOUND_TYPE_WEAPON_RECHARGING)},
                                       {"NPC dying", static_cast<long>(SOUND_TYPE_MONSTER_DYING)},
                                       {"NPC injuring", static_cast<long>(SOUND_TYPE_MONSTER_INJURING)},
                                       {"NPC step", static_cast<long>(SOUND_TYPE_MONSTER_STEP)},
                                       {"NPC talking", static_cast<long>(SOUND_TYPE_MONSTER_TALKING)},
                                       {"NPC attacking", static_cast<long>(SOUND_TYPE_MONSTER_ATTACKING)},
                                       {"NPC eating", static_cast<long>(SOUND_TYPE_MONSTER_EATING)},
                                       {"Anomaly idle", static_cast<long>(SOUND_TYPE_ANOMALY_IDLE)},
                                       {"Object breaking", static_cast<long>(SOUND_TYPE_WORLD_OBJECT_BREAKING)},
                                       {"Object colliding", static_cast<long>(SOUND_TYPE_WORLD_OBJECT_COLLIDING)},
                                       {"Object exploding", static_cast<long>(SOUND_TYPE_WORLD_OBJECT_EXPLODING)},
                                       {"World ambient", static_cast<long>(SOUND_TYPE_WORLD_AMBIENT)},
                                       {0, 0}};
