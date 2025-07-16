////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item script class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "object_factory.h"
#include "object_item_script.h"

ObjectFactory::CLIENT_BASE_CLASS* CObjectItemScript::client_object() const
{
    std::unique_ptr<ObjectFactory::CLIENT_SCRIPT_BASE_CLASS>& ptr = m_client_creator();
    ObjectFactory::CLIENT_SCRIPT_BASE_CLASS* object = ptr.release();
    R_ASSERT(object);

    return object->_construct();
}

ObjectFactory::SERVER_BASE_CLASS* CObjectItemScript::server_object(LPCSTR section) const
{
    std::unique_ptr<ObjectFactory::SERVER_SCRIPT_BASE_CLASS>& ptr = m_server_creator(section);
    ObjectFactory::SERVER_SCRIPT_BASE_CLASS* object = ptr.release();
    R_ASSERT(object);

    ObjectFactory::SERVER_BASE_CLASS* o = object->init();
    R_ASSERT(o);

    return o;
}

CObjectItemScript::CObjectItemScript(sol::function& client_creator, sol::function& server_creator, const CLASS_ID& clsid, LPCSTR script_clsid) : inherited(clsid, script_clsid)
{
    m_client_creator = client_creator;
    m_server_creator = server_creator;
}
