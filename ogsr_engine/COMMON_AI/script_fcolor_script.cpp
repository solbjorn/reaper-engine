////////////////////////////////////////////////////////////////////////////
//	Module 		: script_Fcolor_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float vector script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_fcolor.h"

template <>
void CScriptFcolor::script_register(sol::state_view& lua)
{
    lua.new_usertype<Fcolor>("fcolor", sol::no_constructor, sol::call_constructor, sol::constructors<Fcolor(), Fcolor(float, float, float, float)>(), "r", &Fcolor::r, "g",
                             &Fcolor::g, "b", &Fcolor::b, "a", &Fcolor::a, "set",
                             sol::policies(sol::overload(sol::resolve<Fcolor&(float, float, float, float)>(&Fcolor::set), sol::resolve<Fcolor&(const Fcolor&)>(&Fcolor::set),
                                                         sol::resolve<Fcolor&(u32)>(&Fcolor::set)),
                                           sol::returns_self()));
}
