////////////////////////////////////////////////////////////////////////////
//	Module 		: team_hierarchy_holder_inline.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Team hierarchy holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CTeamHierarchyHolder::CTeamHierarchyHolder(CSeniorityHierarchyHolder* seniority_manager, u32 id) : m_seniority_manager{seniority_manager}, team_id{id}
{
    VERIFY(seniority_manager);
    SeniorityHierarchy::assign_svector(m_squads, max_squad_count, nullptr);
}

IC CSeniorityHierarchyHolder& CTeamHierarchyHolder::team() const
{
    VERIFY(m_seniority_manager);
    return (*m_seniority_manager);
}

IC const CTeamHierarchyHolder::SQUAD_REGISTRY& CTeamHierarchyHolder::squads() const { return (m_squads); }
