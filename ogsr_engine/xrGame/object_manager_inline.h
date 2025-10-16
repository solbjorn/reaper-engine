////////////////////////////////////////////////////////////////////////////
//	Module 		: object_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Object manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename T>
#define CAbstractObjectManager CObjectManager<T>

TEMPLATE_SPECIALIZATION
inline void CAbstractObjectManager::Load(LPCSTR) {}

TEMPLATE_SPECIALIZATION
inline void CAbstractObjectManager::reinit()
{
    m_objects.clear();
    m_selected = nullptr;
}

TEMPLATE_SPECIALIZATION
inline void CAbstractObjectManager::reload(LPCSTR) {}

TEMPLATE_SPECIALIZATION
inline void CAbstractObjectManager::update()
{
    float result = flt_max;
    m_selected = nullptr;

    auto I = m_objects.cbegin();
    auto E = m_objects.cend();
    for (; I != E; ++I)
    {
        float value = do_evaluate(*I);
        if (result > value)
        {
            result = value;
            m_selected = *I;
        }
    }
}

TEMPLATE_SPECIALIZATION
inline float CAbstractObjectManager::do_evaluate(T*) const { return 0.0f; }

TEMPLATE_SPECIALIZATION
inline bool CAbstractObjectManager::is_useful(T* object) const
{
    const ISpatial* self = (const ISpatial*)(object);
    if (!self)
        return (false);

    if ((object->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI)
        return (false);

    return (true);
}

TEMPLATE_SPECIALIZATION
inline bool CAbstractObjectManager::add(T* object)
{
    if (!is_useful(object))
        return (false);

    auto I = std::find(m_objects.begin(), m_objects.end(), object);
    if (m_objects.end() == I)
    {
        m_objects.push_back(object);
        return (true);
    }
    return (true);
}

TEMPLATE_SPECIALIZATION
inline T* CAbstractObjectManager::selected() const { return m_selected; }

TEMPLATE_SPECIALIZATION
inline void CAbstractObjectManager::reset()
{
    m_objects.clear();
    m_selected = nullptr;
}

TEMPLATE_SPECIALIZATION
inline const typename CAbstractObjectManager::OBJECTS& CAbstractObjectManager::objects() const { return m_objects; }

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager
