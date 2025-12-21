////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_object_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife object registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC void CALifeObjectRegistry::add(CSE_ALifeDynamicObject* object)
{
    if (objects().contains(object->ID))
    {
        THROW2((*(objects().find(object->ID))).second == object, "The specified object is already presented in the Object Registry!");
        THROW2((*(objects().find(object->ID))).second != object, "Object with the specified ID is already presented in the Object Registry!");
    }

    const char* name = object->name_replace();
    if (name && name[0])
    {
        ASSERT_FMT(!m_object_ids.contains(name), "%s: duplicate object name '%s'", __FUNCTION__, name);
        m_object_ids.emplace(name, object);
    }

    m_objects.emplace(object->ID, object);
}

IC void CALifeObjectRegistry::remove(const ALife::_OBJECT_ID& id, bool no_assert)
{
    OBJECT_REGISTRY::iterator I = m_objects.find(id);
    if (I == m_objects.end())
    {
        THROW2(no_assert, "The specified object hasn't been found in the Object Registry!");
        return;
    }

    const char* name = I->second->name_replace();
    if (name && name[0])
    {
        auto iter = m_object_ids.find(name);

        ASSERT_FMT(iter != m_object_ids.end(), "%s: no object with name '%s'", __FUNCTION__, name);
        m_object_ids.erase(iter);
    }

    m_objects.erase(I);
}

IC CSE_ALifeDynamicObject* CALifeObjectRegistry::object(const ALife::_OBJECT_ID& id, bool no_assert) const
{
    OBJECT_REGISTRY::const_iterator I = objects().find(id);
    if (objects().end() == I)
    {
#ifdef DEBUG
        if (!no_assert)
            Msg("There is no object with id %d!", id);
#endif

        THROW2(no_assert, "Specified object hasn't been found in the object registry!");
        return nullptr;
    }

    return ((*I).second);
}

inline CSE_ALifeDynamicObject* CALifeObjectRegistry::object_by_name(std::string_view name, bool no_assert) const
{
    const auto it = m_object_ids.find(name);

    if (it == m_object_ids.end())
    {
#ifdef DEBUG
        if (!no_assert)
            Msg("There is no object with name %s!", name.data());
#endif

        THROW2(no_assert, "Specified object hasn't been found in the object registry!");
        return nullptr;
    }

    return it->second;
}

IC const CALifeObjectRegistry::OBJECT_REGISTRY& CALifeObjectRegistry::objects() const { return (m_objects); }

IC CALifeObjectRegistry::OBJECT_REGISTRY& CALifeObjectRegistry::objects() { return (m_objects); }
