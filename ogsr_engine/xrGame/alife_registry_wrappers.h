////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_wrappers.h
//	Created 	: 20.10.2004
//  Modified 	: 20.10.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry wrappers
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_registry_container.h"
#include "alife_registry_wrapper.h"

// typedef CALifeRegistryWrapper<CInfoPortionRegistry> KNOWN_INFO_REGISTRY;

// реестр контактов общения с другими персонажами
//  typedef CALifeRegistryWrapper<CKnownContactsRegistry> KNOWN_CONTACTS_REGISTRY;

// реестр статей энциклопедии, о которых знает актер
//  typedef CALifeRegistryWrapper<CEncyclopediaRegistry> ENCYCLOPEDIA_REGISTRY;

// реестр заданий, полученных актером
//  typedef CALifeRegistryWrapper<CGameTaskRegistry> GAME_TASK_REGISTRY;

// реестр новостей, полученных актером
//  typedef CALifeRegistryWrapper<CGameNewsRegistry> GAME_NEWS_REGISTRY;

template <typename T>
class CALifeRegistryWrapperObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CALifeRegistryWrapperObject<T>);

public:
    T* m_registry;

    IC CALifeRegistryWrapperObject() { m_registry = xr_new<T>(); }

    virtual ~CALifeRegistryWrapperObject() { xr_delete(m_registry); }

    IC T& registry() const
    {
        VERIFY(m_registry);
        return (*m_registry);
    }
};

class CKnownContactsRegistryWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CKnownContactsRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CKnownContactsRegistryWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CKnownContactsRegistry>>);
};

class CEncyclopediaRegistryWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CEncyclopediaRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CEncyclopediaRegistryWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CEncyclopediaRegistry>>);
};

class CGameNewsRegistryWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CGameNewsRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CGameNewsRegistryWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CGameNewsRegistry>>);
};

class CInfoPortionWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CInfoPortionRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CInfoPortionWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CInfoPortionRegistry>>);
};

class CRelationRegistryWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CRelationRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CRelationRegistryWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CRelationRegistry>>);
};

class CMapLocationWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CMapLocationRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CMapLocationWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CMapLocationRegistry>>);
};

class CGameTaskWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CGameTaskRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CGameTaskWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CGameTaskRegistry>>);
};

class CActorStatisticsWrapper : public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CActorStatisticRegistry>>
{
    RTTI_DECLARE_TYPEINFO(CActorStatisticsWrapper, CALifeRegistryWrapperObject<CALifeRegistryWrapper<CActorStatisticRegistry>>);
};
