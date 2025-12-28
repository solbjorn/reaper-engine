////////////////////////////////////////////////////////////////////////////
//	Module 		: script_lanim.cpp
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_lanim.h"

#include "../xr_3da/LightAnimLibrary.h"

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

void lanim_registrator::script_register(sol::state_view& lua)
{
    lua.new_usertype<lanim_wrapper>("color_animator", sol::no_constructor, sol::call_constructor, sol::constructors<lanim_wrapper(LPCSTR)>(), "load", &lanim_wrapper::load,
                                    "calculate", &lanim_wrapper::calculate, "length", &lanim_wrapper::length);
}
