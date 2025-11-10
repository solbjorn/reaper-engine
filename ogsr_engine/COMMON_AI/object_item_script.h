////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item script class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_factory_space.h"
#include "object_item_abstract.h"

class CObjectItemScript : public CObjectItemAbstract
{
    RTTI_DECLARE_TYPEINFO(CObjectItemScript, CObjectItemAbstract);

protected:
    using inherited = CObjectItemAbstract;

    sol::function m_client_creator;
    sol::function m_server_creator;

public:
    explicit CObjectItemScript(sol::function&& client_creator, sol::function&& server_creator, CLASS_ID clsid, LPCSTR script_clsid);

    ObjectFactory::CLIENT_BASE_CLASS* client_object() const override;
    ObjectFactory::SERVER_BASE_CLASS* server_object(LPCSTR section) const override;
};
