////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script4.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"

#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_human_brain.h"

CALifeMonsterBrain* monster_brain(CSE_ALifeMonsterAbstract* monster)
{
    THROW(monster);
    return (&monster->brain());
}

CALifeHumanBrain* human_brain(CSE_ALifeHumanAbstract* human)
{
    THROW(human);
    return (&human->brain());
}

void clear_smart_terrain(CSE_ALifeMonsterAbstract* monster)
{
    THROW(monster);
    monster->m_smart_terrain_id = 0xffff;
}

void smart_terrain_task_activate(CSE_ALifeMonsterAbstract* monster)
{
    THROW(monster);
    monster->m_task_reached = true;
}

void smart_terrain_task_deactivate(CSE_ALifeMonsterAbstract* monster)
{
    THROW(monster);
    monster->m_task_reached = false;
}

ALife::_OBJECT_ID smart_terrain_id(CSE_ALifeMonsterAbstract* monster)
{
    THROW(monster);
    return (monster->m_smart_terrain_id);
}

void CSE_ALifeMonsterAbstract::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeMonsterAbstract>(
        "cse_alife_monster_abstract", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeMonsterAbstract, LPCSTR>), "smart_terrain_id",
        &smart_terrain_id, "m_smart_terrain_id", &CSE_ALifeMonsterAbstract::m_smart_terrain_id, "clear_smart_terrain", &clear_smart_terrain, "brain", &monster_brain, "rank",
        &CSE_ALifeMonsterAbstract::Rank, "smart_terrain_task_activate", &smart_terrain_task_activate, "smart_terrain_task_deactivate", &smart_terrain_task_deactivate,
        sol::base_classes, xr_sol_bases<CSE_ALifeMonsterAbstract>());
}

void CSE_ALifeHumanAbstract::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeHumanAbstract>("cse_alife_human_abstract", sol::no_constructor, sol::call_constructor,
                                                            sol::factories(std::make_unique<CSE_ALifeHumanAbstract, LPCSTR>), "brain", &human_brain,
#ifdef XRGAME_EXPORTS
                                                            "rank", &CSE_ALifeTraderAbstract::Rank, "set_rank", &CSE_ALifeTraderAbstract::SetRank,
#endif
                                                            sol::base_classes, xr_sol_bases<CSE_ALifeHumanAbstract>());
}

void CSE_ALifePsyDogPhantom::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifePsyDogPhantom>("cse_alife_psydog_phantom", sol::no_constructor, sol::call_constructor,
                                                            sol::factories(std::make_unique<CSE_ALifePsyDogPhantom, LPCSTR>), sol::base_classes,
                                                            xr_sol_bases<CSE_ALifePsyDogPhantom>());
}
