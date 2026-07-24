////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item script class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "object_item_script.h"

#include "object_factory.h"

ObjectFactory::CLIENT_BASE_CLASS* CObjectItemScript::client_object() const
{
    sol::userdata data = m_client_creator();
    std::unique_ptr<ObjectFactory::CLIENT_SCRIPT_BASE_CLASS>& ptr = data["factory"](data);
    ObjectFactory::CLIENT_SCRIPT_BASE_CLASS* object = XR_ASSERT_VAL(ptr.release() != nullptr);

    return XR_ASSERT_VAL(object->_construct() != nullptr);
}

ObjectFactory::SERVER_BASE_CLASS* CObjectItemScript::server_object(LPCSTR section) const
{
    sol::userdata data = m_server_creator(section);
    std::unique_ptr<ObjectFactory::SERVER_SCRIPT_BASE_CLASS>& ptr = data["factory"](data);
    ObjectFactory::SERVER_SCRIPT_BASE_CLASS* object = XR_ASSERT_VAL(ptr.release() != nullptr, "", section);

    return XR_ASSERT_VAL(object->init() != nullptr, "", section);
}

CObjectItemScript::CObjectItemScript(sol::function&& client_creator, sol::function&& server_creator, CLASS_ID clsid, LPCSTR script_clsid)
    : inherited{clsid, script_clsid}
{
    m_client_creator = std::move(client_creator);
    m_server_creator = std::move(server_creator);
}
