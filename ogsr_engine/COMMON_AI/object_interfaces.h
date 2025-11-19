////////////////////////////////////////////////////////////////////////////
//	Module 		: object_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife interfaces
////////////////////////////////////////////////////////////////////////////

#pragma once

class NET_Packet;

class XR_NOVTABLE IPureDestroyableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureDestroyableObject);

public:
    ~IPureDestroyableObject() override = 0;

    virtual void destroy() = 0;
};

inline IPureDestroyableObject::~IPureDestroyableObject() = default;

template <typename _storage_type>
class XR_NOVTABLE IPureLoadableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureLoadableObject<_storage_type>);

public:
    ~IPureLoadableObject() override = 0;

    virtual void load(_storage_type& storage) = 0;
};

template <typename _storage_type>
inline IPureLoadableObject<_storage_type>::~IPureLoadableObject() = default;

template <typename _storage_type>
class XR_NOVTABLE IPureSavableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureSavableObject<_storage_type>);

public:
    ~IPureSavableObject() override = 0;

    virtual void save(_storage_type& storage) = 0;
};

template <typename _storage_type>
inline IPureSavableObject<_storage_type>::~IPureSavableObject() = default;

template <typename _storage_type_load, typename _storage_type_save>
class XR_NOVTABLE IPureSerializeObject : public IPureLoadableObject<_storage_type_load>, public IPureSavableObject<_storage_type_save>
{
    RTTI_DECLARE_TYPEINFO(IPureSerializeObject<_storage_type_load, _storage_type_save>, IPureLoadableObject<_storage_type_load>, IPureSavableObject<_storage_type_save>);

public:
    ~IPureSerializeObject() override = 0;
};

template <typename _storage_type_load, typename _storage_type_save>
inline IPureSerializeObject<_storage_type_load, _storage_type_save>::~IPureSerializeObject() = default;

class XR_NOVTABLE IPureServerObject : public IPureSerializeObject<IReader, IWriter>
{
    RTTI_DECLARE_TYPEINFO(IPureServerObject, IPureSerializeObject<IReader, IWriter>);

public:
    ~IPureServerObject() override = 0;

    virtual void __STATE_Write(NET_Packet& tNetPacket) = 0;
    virtual void __STATE_Read(NET_Packet& tNetPacket, u16 size) = 0;
    virtual void UPDATE_Write(NET_Packet& tNetPacket) = 0;
    virtual void UPDATE_Read(NET_Packet& tNetPacket) = 0;
};

inline IPureServerObject::~IPureServerObject() = default;

class XR_NOVTABLE IPureSchedulableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureSchedulableObject);

public:
    ~IPureSchedulableObject() override = 0;

    virtual void update() = 0;
};

inline IPureSchedulableObject::~IPureSchedulableObject() = default;
