////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_client_server_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client and server class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _client_type, typename _server_type>
#define CSObjectItemClientServer CObjectItemClientServer<_client_type, _server_type>

TEMPLATE_SPECIALIZATION
IC CSObjectItemClientServer::CObjectItemClientServer(const CLASS_ID& clsid, LPCSTR script_clsid) : inherited(clsid, script_clsid) {}

TEMPLATE_SPECIALIZATION
ObjectFactory::CLIENT_BASE_CLASS* CSObjectItemClientServer::client_object() const { return XR_ASSERT_VAL(xr_new<CLIENT_TYPE>()->_construct() != nullptr); }

TEMPLATE_SPECIALIZATION
ObjectFactory::SERVER_BASE_CLASS* CSObjectItemClientServer::server_object(LPCSTR section) const
{
    return XR_ASSERT_VAL(xr_new<SERVER_TYPE>(section)->init() != nullptr, "", section);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSObjectItemClientServer

#define TEMPLATE_SPECIALIZATION template <typename _client_type_single, typename _client_type_mp, typename _server_type_single, typename _server_type_mp>
#define CSObjectItemClientServerSingleMp CObjectItemClientServerSingleMp<_client_type_single, _client_type_mp, _server_type_single, _server_type_mp>

TEMPLATE_SPECIALIZATION
IC CSObjectItemClientServerSingleMp::CObjectItemClientServerSingleMp(const CLASS_ID& clsid, LPCSTR script_clsid) : inherited(clsid, script_clsid) {}

TEMPLATE_SPECIALIZATION
ObjectFactory::CLIENT_BASE_CLASS* CSObjectItemClientServerSingleMp::client_object() const
{
    return XR_ASSERT_VAL(xr_new<_client_type_single>()->_construct() != nullptr);
}

TEMPLATE_SPECIALIZATION
ObjectFactory::SERVER_BASE_CLASS* CSObjectItemClientServerSingleMp::server_object(LPCSTR section) const
{
    return XR_ASSERT_VAL(xr_new<_server_type_single>(section)->init() != nullptr, "", section);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSObjectItemClientServerSingleMp
