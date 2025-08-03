////////////////////////////////////////////////////////////////////////////
//	Module 		: property_storage_script.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property storage class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "property_storage.h"

void CPropertyStorage::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPropertyStorage>("property_storage", sol::no_constructor, sol::call_constructor, sol::constructors<CPropertyStorage()>(), "set_property",
                                       &CPropertyStorage::set_property, "property", &CPropertyStorage::property);
}
