////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_corpse_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent corpse manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "agent_corpse_manager.h"

#include "member_order.h"
#include "ai/stalker/ai_stalker.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "agent_manager.h"
#include "agent_member_manager.h"

struct CRemoveMemberCorpsesPredicate final
{
    IC bool operator()(CMemberCorpse& corpse) const { return (!!corpse.reactor()); }
};

struct CRemoveOfflineCorpsesPredicate final
{
    CObject* m_object;
    IC CRemoveOfflineCorpsesPredicate(CObject* object)
    {
        VERIFY(object);
        m_object = object;
    }

    IC bool operator()(CMemberCorpse& corpse) const { return (corpse.corpse()->ID() == m_object->ID()); }
};

bool CAgentCorpseManager::process_corpse(CMemberOrder& member)
{
    float min_dist_sqr{std::numeric_limits<float>::max()};
    CMemberCorpse* best_corpse{nullptr};

    for (auto& corp : m_corpses)
    {
        if (!member.object().memory().visual().visible_now(corp.corpse()))
            continue;

        if (const f32 dist_sqr = corp.corpse()->Position().distance_to_sqr(member.object().Position()); dist_sqr < min_dist_sqr)
        {
            if (corp.reactor() != nullptr && corp.reactor()->Position().distance_to_sqr(corp.corpse()->Position()) <= min_dist_sqr)
                continue;

            min_dist_sqr = dist_sqr;
            best_corpse = &corp;
        }
    }

    if (best_corpse == nullptr)
        return false;

    best_corpse->reactor(&member.object());

    return true;
}

void CAgentCorpseManager::react_on_member_death()
{
    for (;;)
    {
        bool changed = false;
        CAgentMemberManager::MEMBER_STORAGE::iterator I = object().member().combat_members().begin();
        CAgentMemberManager::MEMBER_STORAGE::iterator E = object().member().combat_members().end();
        for (; I != E; ++I)
            if (!(*I)->member_death_reaction().m_processing)
                changed = process_corpse(**I);

        if (!changed)
            break;
    }

    {
        MEMBER_CORPSES::iterator I = m_corpses.begin();
        MEMBER_CORPSES::iterator E = m_corpses.end();
        for (; I != E; ++I)
        {
            if (!(*I).reactor())
                continue;

            CMemberOrder::CMemberDeathReaction& reaction = object().member().member((*I).reactor()).member_death_reaction();
            reaction.m_member = (*I).corpse();
            reaction.m_time = (*I).time();
            reaction.m_processing = true;
        }

        m_corpses.erase(std::remove_if(m_corpses.begin(), m_corpses.end(), CRemoveMemberCorpsesPredicate()), m_corpses.end());
    }
}

void CAgentCorpseManager::remove_links(CObject* object)
{
    m_corpses.erase(std::remove_if(m_corpses.begin(), m_corpses.end(), CRemoveOfflineCorpsesPredicate(object)), m_corpses.end());
}

void CAgentCorpseManager::update() {}
