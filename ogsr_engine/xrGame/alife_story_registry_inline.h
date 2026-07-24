////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_story_registry_inline.h
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife story registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC void CALifeStoryRegistry::remove(ALife::_STORY_ID id, bool no_assert)
{
    if (id == INVALID_STORY_ID)
        return;

    const auto I = m_objects.find(id);
    if (I == m_objects.end())
    {
        XR_ASSERT(no_assert, "story object not found in the registry", id);
        return;
    }

    m_objects.erase(I);
}

IC const CALifeStoryRegistry::STORY_REGISTRY& CALifeStoryRegistry::objects() const { return (m_objects); }

IC CSE_ALifeDynamicObject* CALifeStoryRegistry::object(ALife::_STORY_ID id, bool no_assert) const
{
    if (id == INVALID_STORY_ID)
        return nullptr;

    const auto I = m_objects.find(id);
    if (I == m_objects.end())
    {
        XR_ASSERT(no_assert, "story object not found in the registry", id);
        return nullptr;
    }

    return I->second;
}
