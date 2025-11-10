////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_object_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife object registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_ALife.h"
#include "profiler.h"

class CALifeObjectRegistry : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CALifeObjectRegistry);

public:
    using OBJECT_REGISTRY = xr_map<ALife::_OBJECT_ID, CSE_ALifeDynamicObject*>;

protected:
    OBJECT_REGISTRY m_objects;
    string_unordered_map<shared_str, CSE_ALifeDynamicObject*> m_object_ids;

private:
    void save(IWriter& memory_stream, CSE_ALifeDynamicObject* object, u32& object_count);

public:
    static CSE_ALifeDynamicObject* get_object(IReader& file_stream);

    CALifeObjectRegistry();
    virtual ~CALifeObjectRegistry();

    virtual void save(IWriter& memory_stream);
    void load(IReader& file_stream);

    IC void add(CSE_ALifeDynamicObject* object);
    IC void remove(const ALife::_OBJECT_ID& id, bool no_assert = false);

    IC CSE_ALifeDynamicObject* object(const ALife::_OBJECT_ID& id, bool no_assert = false) const;
    inline CSE_ALifeDynamicObject* object_by_name(absl::string_view name, bool no_assert = false) const;

    IC const OBJECT_REGISTRY& objects() const;
    IC OBJECT_REGISTRY& objects();
};

#include "alife_object_registry_inline.h"
