////////////////////////////////////////////////////////////////////////////
//	Module 		: location_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Location manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"

class CGameObject;

class CLocationManager : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CLocationManager);

private:
    GameGraph::TERRAIN_VECTOR m_vertex_types;
    CGameObject* m_object;

public:
    IC CLocationManager(CGameObject* object);
    virtual ~CLocationManager();
    virtual void Load(LPCSTR section);
    virtual void reload(LPCSTR section);
    IC const GameGraph::TERRAIN_VECTOR& vertex_types() const;
};

#include "location_manager_inline.h"
