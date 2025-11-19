////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_client_server.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client and server class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_factory_space.h"
#include "object_item_abstract.h"
#include "object_factory.h"

template <typename _client_type, typename _server_type>
class CObjectItemClientServer : public CObjectItemAbstract
{
    RTTI_DECLARE_TYPEINFO(CObjectItemClientServer<_client_type, _server_type>, CObjectItemAbstract);

protected:
    typedef CObjectItemAbstract inherited;
    typedef _client_type CLIENT_TYPE;
    typedef _server_type SERVER_TYPE;

public:
    inline explicit CObjectItemClientServer(const CLASS_ID& clsid, LPCSTR script_clsid);
    ~CObjectItemClientServer() override = default;

    virtual ObjectFactory::CLIENT_BASE_CLASS* client_object() const;
    virtual ObjectFactory::SERVER_BASE_CLASS* server_object(LPCSTR section) const;
};

template <typename _client_type_single, typename _client_type_mp, typename _server_type_single, typename _server_type_mp>
class CObjectItemClientServerSingleMp : public CObjectItemAbstract
{
    RTTI_DECLARE_TYPEINFO(CObjectItemClientServerSingleMp<_client_type_single, _client_type_mp, _server_type_single, _server_type_mp>, CObjectItemAbstract);

public:
    typedef CObjectItemAbstract inherited;

    inline explicit CObjectItemClientServerSingleMp(const CLASS_ID& clsid, LPCSTR script_clsid);
    ~CObjectItemClientServerSingleMp() override = default;

    virtual ObjectFactory::CLIENT_BASE_CLASS* client_object() const;
    virtual ObjectFactory::SERVER_BASE_CLASS* server_object(LPCSTR section) const;
};

#include "object_item_client_server_inline.h"
