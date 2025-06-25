////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container.h
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_registry_container_space.h"
#include "alife_registry_container_composition.h"
#include "alife_abstract_registry.h"

template <typename T>
struct CLinearRegistryType;

template <typename... Ts>
struct CLinearRegistryType<imdexlib::typelist<Ts...>> : Ts...
{
    RTTI_DECLARE_TYPEINFO(CLinearRegistryType<imdexlib::typelist<Ts...>>, Ts...);
};

class CALifeRegistryContainer : public CLinearRegistryType<registry_type_list>
{
    RTTI_DECLARE_TYPEINFO(CALifeRegistryContainer, CLinearRegistryType<registry_type_list>);

public:
    using TYPE_LIST = registry_type_list;

    template <typename T>
    IC T& operator()(const T*);
    template <typename T>
    IC const T& operator()(const T*) const;
    virtual void load(IReader& file_stream);
    virtual void save(IWriter& memory_stream);
};

#include "alife_registry_container_inline.h"
