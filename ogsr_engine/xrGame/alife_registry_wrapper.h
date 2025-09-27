////////////////////////////////////////////////////////////////////////////
// alife_registry_wrapper.h - обертка для реестра, предусматривающая работу
//							  без alife()
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"
#include "alife_simulator.h"

template <typename _registry_type>
class CALifeRegistryWrapper : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CALifeRegistryWrapper<_registry_type>);

public:
    IC CALifeRegistryWrapper() = default;
    virtual ~CALifeRegistryWrapper() { delete_data(local_registry); }

    IC void init(u16 id) { holder_id = id; }

    typename _registry_type::_data& objects();
    const typename _registry_type::_data* objects_ptr();

    typename _registry_type::_data& objects(u16 id);
    const typename _registry_type::_data* objects_ptr(u16 id);

    IC const typename _registry_type::OBJECT_REGISTRY& get_registry_objects() const;
    IC typename _registry_type::OBJECT_REGISTRY& get_registry_objects();

private:
    // id - владельца реестра
    u16 holder_id{std::numeric_limits<u16>::max()};

    // реестр на случай, если нет ALife (для отладки)
    //	typename _registry_type::_data	local_registry;
    typename _registry_type::OBJECT_REGISTRY local_registry;
};

template <typename _registry_type>
IC const typename _registry_type::OBJECT_REGISTRY& CALifeRegistryWrapper<_registry_type>::get_registry_objects() const
{
    return ai().alife().registry(static_cast<_registry_type*>(nullptr)).objects();
}

template <typename _registry_type>
IC typename _registry_type::OBJECT_REGISTRY& CALifeRegistryWrapper<_registry_type>::get_registry_objects()
{
    return ai().alife().registry(static_cast<_registry_type*>(nullptr)).objects();
}

template <typename _registry_type>
const typename _registry_type::_data* CALifeRegistryWrapper<_registry_type>::objects_ptr(u16 id)
{
    if (!ai().get_alife())
    {
        typename _registry_type::iterator I = local_registry.find(id);
        if (I == local_registry.end())
        {
            typename _registry_type::_data new_registry;
            auto [iter, inserted] = local_registry.try_emplace(id, new_registry);
            VERIFY(inserted);

            return &(iter->second);
        }

        return (&(*I).second);
    }

    VERIFY(0xffff != id);

    typename _registry_type::_data* registy_container = ai().alife().registry(static_cast<_registry_type*>(nullptr)).object(id, true);
    return registy_container;
}

template <typename _registry_type>
typename _registry_type::_data& CALifeRegistryWrapper<_registry_type>::objects(u16 id)
{
    if (!ai().get_alife())
    {
        typename _registry_type::iterator I = local_registry.find(id);
        if (I == local_registry.end())
        {
            typename _registry_type::_data new_registry;
            auto [iter, inserted] = local_registry.try_emplace(id, new_registry);
            VERIFY(inserted);

            return (iter->second);
        }

        return ((*I).second);
    }

    typename _registry_type::_data* registy_container = ai().alife().registry(static_cast<_registry_type*>(nullptr)).object(id, true);

    if (!registy_container)
    {
        typename _registry_type::_data new_registry;
        ai().alife().registry(static_cast<_registry_type*>(nullptr)).add(id, new_registry, false);
        registy_container = ai().alife().registry(static_cast<_registry_type*>(nullptr)).object(id, true);
        VERIFY(registy_container);
    }

    return *registy_container;
}

template <typename _registry_type>
const typename _registry_type::_data* CALifeRegistryWrapper<_registry_type>::objects_ptr()
{
    return objects_ptr(holder_id);
}

template <typename _registry_type>
typename _registry_type::_data& CALifeRegistryWrapper<_registry_type>::objects()
{
    return objects(holder_id);
}
