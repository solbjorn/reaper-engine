////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "object_factory.h"
#include "ai_space.h"
#include "script_engine.h"
#include "object_item_script.h"

void CObjectFactory::register_script_class(LPCSTR client_class, LPCSTR server_class, LPCSTR clsid, LPCSTR script_clsid)
{
    sol::function client;

    if (!ai().script_engine().function(client_class, client))
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "Cannot register class %s", client_class);
        return;
    }

    sol::function server;

    if (!ai().script_engine().function(server_class, server))
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "Cannot register class %s", server_class);
        return;
    }

    add(xr_new<CObjectItemScript>(client, server, TEXT2CLSID(clsid), script_clsid));
}

void CObjectFactory::register_script_class(LPCSTR unknown_class, LPCSTR clsid, LPCSTR script_clsid)
{
    sol::function creator;

    if (!ai().script_engine().function(unknown_class, creator))
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "Cannot register class %s", unknown_class);
        return;
    }

    add(xr_new<CObjectItemScript>(creator, creator, TEXT2CLSID(clsid), script_clsid));
}

void CObjectFactory::register_script_classes() { ai(); }

void CObjectFactory::register_script() const
{
    actualize();

    sol::state_view lua(ai().script_engine().lua());
    sol::table target = lua.create_table(clsids().size(), 0);

    size_t id = 0;
    for (const auto& item : clsids())
        target.set(*item->script_clsid(), id++);

    xr::sol_new_enum(lua, "clsid", target);
}

void CObjectFactory::script_register(sol::state_view& lua)
{
    lua.new_usertype<CObjectFactory>("object_factory", sol::no_constructor, "register",
                                     sol::overload(sol::resolve<void(LPCSTR, LPCSTR, LPCSTR, LPCSTR)>(&CObjectFactory::register_script_class),
                                                   sol::resolve<void(LPCSTR, LPCSTR, LPCSTR)>(&CObjectFactory::register_script_class)));
}
