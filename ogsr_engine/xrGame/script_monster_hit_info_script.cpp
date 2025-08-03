#include "stdafx.h"
#include "script_monster_hit_info.h"
#include "script_game_object.h"
#include "ai_monster_space.h"
#include "AI/Monsters/monster_sound_defs.h"

void CScriptMonsterHitInfo::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptMonsterHitInfo>("MonsterHitInfo", sol::no_constructor, "who", &CScriptMonsterHitInfo::who, "direction", &CScriptMonsterHitInfo::direction, "time",
                                            &CScriptMonsterHitInfo::time);

    lua.new_enum("MonsterSpace",
                 // sounds
                 "sound_script", MonsterSound::eMonsterSoundScript,

                 // head_anim
                 "head_anim_normal", MonsterSpace::eHeadAnimNormal, "head_anim_angry", MonsterSpace::eHeadAnimAngry, "head_anim_glad", MonsterSpace::eHeadAnimGlad,
                 "head_anim_kind", MonsterSpace::eHeadAnimKind);
}
