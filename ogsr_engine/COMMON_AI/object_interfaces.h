////////////////////////////////////////////////////////////////////////////
//	Module 		: object_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife interfaces
////////////////////////////////////////////////////////////////////////////

#pragma once

class NET_Packet;

class IPureDestroyableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureDestroyableObject);

public:
    virtual void destroy() = 0;
};

template <typename _storage_type>
class IPureLoadableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureLoadableObject<_storage_type>);

public:
    virtual void load(_storage_type& storage) = 0;
};

template <typename _storage_type>
class IPureSavableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureSavableObject<_storage_type>);

public:
    virtual void save(_storage_type& storage) = 0;
};

template <typename _storage_type_load, typename _storage_type_save>
class IPureSerializeObject : public IPureLoadableObject<_storage_type_load>, public IPureSavableObject<_storage_type_save>
{
    RTTI_DECLARE_TYPEINFO(IPureSerializeObject<_storage_type_load, _storage_type_save>, IPureLoadableObject<_storage_type_load>, IPureSavableObject<_storage_type_save>);
};

class IPureServerObject : public IPureSerializeObject<IReader, IWriter>
{
    RTTI_DECLARE_TYPEINFO(IPureServerObject, IPureSerializeObject<IReader, IWriter>);

public:
    virtual void STATE_Write(NET_Packet& tNetPacket) = 0;
    virtual void STATE_Read(NET_Packet& tNetPacket, u16 size) = 0;
    virtual void UPDATE_Write(NET_Packet& tNetPacket) = 0;
    virtual void UPDATE_Read(NET_Packet& tNetPacket) = 0;
};

class IPureSchedulableObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPureSchedulableObject);

public:
    virtual void update() = 0;
};
