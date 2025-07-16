////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file.cpp
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "../xr_3da/LightAnimLibrary.h"

#include "script_lanim.h"

struct lanim_wrapper
{
private:
    CLAItem* item;

public:
    lanim_wrapper(LPCSTR name) { load(name); }
    void load(LPCSTR name)
    {
        item = LALib.FindItem(name);
        R_ASSERT3(item, "Can't find color anim:", name);
    }
    u32 length()
    {
        VERIFY(item);
        return item->Length_ms();
    }
    Fcolor calculate(float T)
    {
        int frame;
        VERIFY(item);
        return Fcolor().set(item->CalculateRGB(T, frame));
    }
};

void lanim_registrator::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<lanim_wrapper>("color_animator", sol::no_constructor, sol::call_constructor, sol::constructors<lanim_wrapper(LPCSTR)>(), "load",
                                                   &lanim_wrapper::load, "calculate", &lanim_wrapper::calculate, "length", &lanim_wrapper::length);
}
