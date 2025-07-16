////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "script_effector.h"

static void SPPInfo_assign(SPPInfo* self, SPPInfo* obj) { *self = *obj; }

static void add_effector(CScriptEffector* self) { self->Add(); }
static void remove_effector(CScriptEffector* self) { self->Remove(); }

void CScriptEffector::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<SPPInfo::SDuality>("duality", sol::no_constructor, sol::call_constructor, sol::constructors<SPPInfo::SDuality(), SPPInfo::SDuality(float, float)>(), "h",
                                        &SPPInfo::SDuality::h, "v", &SPPInfo::SDuality::v, "set", &SPPInfo::SDuality::set);

    lua.new_usertype<SPPInfo::SColor>("color", sol::no_constructor, sol::call_constructor, sol::constructors<SPPInfo::SColor(), SPPInfo::SColor(float, float, float)>(), "r",
                                      &SPPInfo::SColor::r, "g", &SPPInfo::SColor::g, "b", &SPPInfo::SColor::b, "set", &SPPInfo::SColor::set);

    lua.new_usertype<SPPInfo::SNoise>("noise", sol::no_constructor, sol::call_constructor, sol::constructors<SPPInfo::SNoise(), SPPInfo::SNoise(float, float, float)>(),
                                      "intensity", &SPPInfo::SNoise::intensity, "grain", &SPPInfo::SNoise::grain, "fps", &SPPInfo::SNoise::fps, "set", &SPPInfo::SNoise::set);

    lua.new_usertype<SPPInfo>("effector_params", sol::no_constructor, sol::call_constructor, sol::constructors<SPPInfo()>(), "blur", &SPPInfo::blur, "gray", &SPPInfo::gray, "dual",
                              &SPPInfo::duality, "noise", &SPPInfo::noise, "color_base", &SPPInfo::color_base, "color_gray", &SPPInfo::color_gray, "color_add", &SPPInfo::color_add,
                              "assign", &SPPInfo_assign);

    lua.new_usertype<CScriptEffector>("effector", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptEffector(int, float)>(), "start", &add_effector, "finish",
                                      &remove_effector, "process", &CScriptEffector::process);
}
