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
    if (const auto it = objects().find(object->ID); it != objects().end())
    {
        XR_ASSERT(it->second == object, "object already registered", object->ID);
        XR_ASSERT(it->second != object, "object with this ID already registered", object->ID);
    }

    if (const auto name = object->name_replace(); name != nullptr && name[0] != '\0')
    {
        XR_ASSERT(!m_object_ids.contains(name), "duplicate object name", name, object->ID);
        m_object_ids.emplace(name, object);
    }

    m_objects.emplace(object->ID, object);
}

IC void CALifeObjectRegistry::remove(const ALife::_OBJECT_ID& id, bool no_assert)
{
    const auto I = m_objects.find(id);
    if (I == m_objects.end())
    {
        XR_ASSERT(no_assert, "object not found in the registry", id);
        return;
    }

    if (const auto name = I->second->name_replace(); name != nullptr && name[0] != '\0')
        m_object_ids.erase(XR_ASSERT_VAL(m_object_ids.find(name) != m_object_ids.end(), "no object with such name", name, id));

    m_objects.erase(I);
}

IC CSE_ALifeDynamicObject* CALifeObjectRegistry::object(const ALife::_OBJECT_ID& id, bool no_assert) const
{
    const auto I = objects().find(id);
    if (I == objects().end())
    {
        XR_ASSERT(no_assert, "object not found in the registry", id);
        return nullptr;
    }

    return I->second;
}

inline CSE_ALifeDynamicObject* CALifeObjectRegistry::object_by_name(std::string_view name, bool no_assert) const
{
    const auto it = m_object_ids.find(name);
    if (it == m_object_ids.end())
    {
        XR_ASSERT(no_assert, "object not found in the registry", name);
        return nullptr;
    }

    return it->second;
}

IC const CALifeObjectRegistry::OBJECT_REGISTRY& CALifeObjectRegistry::objects() const { return (m_objects); }
IC CALifeObjectRegistry::OBJECT_REGISTRY& CALifeObjectRegistry::objects() { return (m_objects); }
