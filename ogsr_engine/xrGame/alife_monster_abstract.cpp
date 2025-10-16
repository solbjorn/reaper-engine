////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_abstract.cpp
//	Created 	: 27.10.2005
//  Modified 	: 27.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife mnster abstract class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_group_registry.h"
#include "relation_registry.h"
#include "alife_time_manager.h"
#include "alife_graph_registry.h"
#include "game_graph.h"
#include "alife_object_registry.h"
#include "ef_storage.h"
#include "ef_pattern.h"
#include "alife_monster_brain.h"
#include "alife_monster_movement_manager.h"
#include "alife_monster_detail_path_manager.h"

void CSE_ALifeMonsterAbstract::add_online(const bool& update_registries)
{
    inherited1::add_online(update_registries);
    brain().on_switch_online();
}

void CSE_ALifeMonsterAbstract::add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries)
{
    inherited1::add_offline(saved_children, update_registries);
    brain().on_switch_offline();
}

void CSE_ALifeMonsterAbstract::__on_register()
{
    inherited1::__on_register();
    brain().on_register();
}

void CSE_ALifeMonsterAbstract::__on_unregister()
{
    inherited1::__on_unregister();
    if (!ai().alife().is_unloading())
        RELATION_REGISTRY().ClearRelations(ID);
    brain().on_unregister();
    if (m_group_id != 0xffff)
        ai().alife().groups().object(m_group_id).unregister_member(ID);
}

void CSE_ALifeMonsterAbstract::update()
{
    if (!bfActive())
        return;

    brain().update();
}

CSE_ALifeItemWeapon* CSE_ALifeMonsterAbstract::tpfGetBestWeapon(ALife::EHitType& tHitType, float& fHitPower)
{
    m_tpCurrentBestWeapon = nullptr;
    fHitPower = m_fHitPower;
    tHitType = m_tHitType;

    return m_tpCurrentBestWeapon;
}

ALife::EMeetActionType CSE_ALifeMonsterAbstract::tfGetActionType(CSE_ALifeSchedulable*, int, bool) { return ALife::eMeetActionTypeIgnore; }

bool CSE_ALifeMonsterAbstract::bfActive()
{
    CSE_ALifeGroupAbstract* l_tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(this);
    return (interactive() && ((l_tpALifeGroupAbstract && (l_tpALifeGroupAbstract->m_wCount > 0)) || (!l_tpALifeGroupAbstract && (fHealth > EPS_L))));
}

CSE_ALifeDynamicObject* CSE_ALifeMonsterAbstract::tpfGetBestDetector()
{
    CSE_ALifeGroupAbstract* l_tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(this);
    if (!l_tpALifeGroupAbstract)
        return this;

    if (!l_tpALifeGroupAbstract->m_wCount)
        return nullptr;

    return ai().alife().objects().object(l_tpALifeGroupAbstract->m_tpMembers[0]);
}

Fvector CSE_ALifeMonsterAbstract::draw_level_position() const { return brain().movement().detail().draw_level_position(); }

bool CSE_ALifeMonsterAbstract::redundant() const
{
    if (g_Alive())
        return (false);

    if (m_bOnline)
        return (false);

    if (m_story_id != INVALID_STORY_ID)
        return (false);

    if (!m_game_death_time)
        return (false);

    ALife::_TIME_ID current_time = alife().time_manager().game_time();
    VERIFY(m_game_death_time <= current_time);
    if ((m_game_death_time + m_stay_after_death_time_interval) > current_time)
        return (false);

    return (true);
}
