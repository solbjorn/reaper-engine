////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_storage.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path storage
////////////////////////////////////////////////////////////////////////////

#pragma once

class CPatrolPath;
class CLevelGraph;
class CGameLevelCrossTable;
class CGameGraph;

#include "object_interfaces.h"
#include "associative_vector.h"

class CPatrolPathStorage : public IPureSerializeObject<IReader, IWriter>
{
    RTTI_DECLARE_TYPEINFO(CPatrolPathStorage, IPureSerializeObject<IReader, IWriter>);

private:
    typedef IPureSerializeObject<IReader, IWriter> inherited;

public:
    typedef associative_vector<shared_str, CPatrolPath*> PATROL_REGISTRY;
    typedef PATROL_REGISTRY::iterator iterator;
    typedef PATROL_REGISTRY::const_iterator const_iterator;

protected:
    PATROL_REGISTRY m_registry;

public:
    IC CPatrolPathStorage();
    virtual ~CPatrolPathStorage();
    virtual void load(IReader& stream);
    virtual void save(IWriter& stream);

public:
    void load_raw(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph, IReader& stream);
    void append_from_ini(CInifile& way_inifile);
    IC const CPatrolPath* path(shared_str patrol_name, bool no_assert = false) const;
    IC const PATROL_REGISTRY& patrol_paths() const;
    void add_path(shared_str, CPatrolPath*);
    void remove_path(shared_str);

    const CPatrolPath* safe_path(shared_str, bool = false, bool = false) const;
};

#include "patrol_path_storage_inline.h"
