////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_schedule_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife schedule registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_ALife.h"

#include "safe_map_iterator.h"
#include "ai_debug.h"

class CALifeScheduleRegistry : public CSafeMapIterator<ALife::_OBJECT_ID, CSE_ALifeSchedulable, std::less<ALife::_OBJECT_ID>, false>
{
    RTTI_DECLARE_TYPEINFO(CALifeScheduleRegistry, CSafeMapIterator<ALife::_OBJECT_ID, CSE_ALifeSchedulable, std::less<ALife::_OBJECT_ID>, false>);

private:
    struct CUpdatePredicate
    {
        u32 m_count;
        mutable u32 m_current{};

        CUpdatePredicate(u32 count) : m_count{count} {}

        bool operator()(_iterator& i, u64 cycle_count, bool) const
        {
            if ((*i).second->m_schedule_counter == cycle_count)
                return (false);

            if (m_current >= m_count)
                return (false);

            ++m_current;
            (*i).second->m_schedule_counter = cycle_count;

            return (true);
        }

        void operator()(_iterator& i, u64) const { (*i).second->update(); }
    };

protected:
    typedef CSafeMapIterator<ALife::_OBJECT_ID, CSE_ALifeSchedulable, std::less<ALife::_OBJECT_ID>, false> inherited;

    u32 m_objects_per_update{1};

public:
    CALifeScheduleRegistry() = default;
    ~CALifeScheduleRegistry() override;

    void add(CSE_ALifeDynamicObject* object);
    void remove(CSE_ALifeDynamicObject* object, bool no_assert = false);

    inline void update();
    IC CSE_ALifeSchedulable* object(const ALife::_OBJECT_ID& id, bool no_assert = false) const;
    inline void objects_per_update(u32 objects_per_update);
};

#include "alife_schedule_registry_inline.h"
