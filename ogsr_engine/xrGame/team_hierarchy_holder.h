////////////////////////////////////////////////////////////////////////////
//	Module 		: team_hierarchy_holder.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Team hierarchy holder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "seniority_hierarchy_space.h"

class CSquadHierarchyHolder;
class CSeniorityHierarchyHolder;

class CTeamHierarchyHolder : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CTeamHierarchyHolder);

private:
    enum
    {
        max_squad_count = 256
    };

private:
    typedef svector<CSquadHierarchyHolder*, max_squad_count> SQUAD_REGISTRY;

private:
    CSeniorityHierarchyHolder* m_seniority_manager;
    mutable SQUAD_REGISTRY m_squads;
    u32 team_id;

public:
    inline explicit CTeamHierarchyHolder(CSeniorityHierarchyHolder* m_seniority_manager, u32);
    ~CTeamHierarchyHolder() override;

    CSquadHierarchyHolder& squad(u32 squad_id) const;
    IC CSeniorityHierarchyHolder& team() const;
    IC const SQUAD_REGISTRY& squads() const;
    IC u32 id() const { return team_id; }
};

#include "team_hierarchy_holder_inline.h"
