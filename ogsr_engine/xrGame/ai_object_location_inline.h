////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

inline CAI_ObjectLocation::CAI_ObjectLocation() { init(); }
inline void CAI_ObjectLocation::reinit() { init(); }

inline GameGraph::_GRAPH_ID CAI_ObjectLocation::game_vertex_id() const { return m_game_vertex_id; }
inline u32 CAI_ObjectLocation::level_vertex_id() const { return m_level_vertex_id; }
