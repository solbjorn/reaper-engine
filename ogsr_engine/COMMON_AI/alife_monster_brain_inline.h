////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_brain_inline.h
//	Created 	: 06.10.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster brain class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CALifeMonsterBrain::object_type& CALifeMonsterBrain::object() const { return *XR_ASSERT_VAL(m_object != nullptr); }
IC CALifeMonsterBrain::movement_manager_type& CALifeMonsterBrain::movement() const { return *XR_ASSERT_VAL(m_movement_manager != nullptr); }

inline bool CALifeMonsterBrain::can_choose_alife_tasks() const { return m_can_choose_alife_tasks; }
inline void CALifeMonsterBrain::can_choose_alife_tasks(bool value) { m_can_choose_alife_tasks = value; }
