////////////////////////////////////////////////////////////////////////////
//	Module 		: script_fmatrix_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float matrix script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_fmatrix.h"

static void get_matrix_hpb(const Fmatrix& self, float* h, float* p, float* b) { self.getHPB(*h, *p, *b); }
static void get_matrix_xyzi(const Fmatrix& self, float* x, float* y, float* z) { self.getXYZi(*x, *y, *z); }

template <>
void CScriptFmatrix::script_register(sol::state_view& lua)
{
    lua.new_usertype<Fmatrix>(
        "matrix", sol::no_constructor, sol::call_constructor, sol::initializers([](Fmatrix& m) { m = Fmatrix{}; }), "i", &Fmatrix::i, "_14_",
        sol::property(&Fmatrix::get_14, &Fmatrix::set_14), "j", &Fmatrix::j, "_24_", sol::property(&Fmatrix::get_24, &Fmatrix::set_24), "k", &Fmatrix::k, "_34_",
        sol::property(&Fmatrix::get_34, &Fmatrix::set_34), "c", &Fmatrix::c, "_44_", sol::property(&Fmatrix::get_44, &Fmatrix::set_44), "set",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(const Fmatrix&)>(&Fmatrix::set),
                                    sol::resolve<Fmatrix&(const Fvector&, const Fvector&, const Fvector&, const Fvector&)>(&Fmatrix::set)),
                      sol::returns_self()),
        "identity", sol::policies(&Fmatrix::identity, sol::returns_self()), "mk_xform", sol::policies(&Fmatrix::mk_xform, sol::returns_self()), "build_camera_dir",
        sol::policies(&Fmatrix::build_camera_dir, sol::returns_self()), "build_projection", sol::policies(&Fmatrix::build_projection, sol::returns_self()), "mulA_43",
        sol::policies(&Fmatrix::mulA_43, sol::returns_self()), "mulA_44", sol::policies(&Fmatrix::mulA_44, sol::returns_self()), "mulB_43",
        sol::policies(&Fmatrix::mulB_43, sol::returns_self()), "mulB_44", sol::policies(&Fmatrix::mulB_44, sol::returns_self()), "mul_43",
        sol::policies(&Fmatrix::mul_43, sol::returns_self()), "translate",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(float, float, float)>(&Fmatrix::translate), sol::resolve<Fmatrix&(const Fvector&)>(&Fmatrix::translate)),
                      sol::returns_self()),
        "translate_add",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(float, float, float)>(&Fmatrix::translate_add), sol::resolve<Fmatrix&(const Fvector&)>(&Fmatrix::translate_add)),
                      sol::returns_self()),
        "translate_over",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(float, float, float)>(&Fmatrix::translate_over), sol::resolve<Fmatrix&(const Fvector&)>(&Fmatrix::translate_over)),
                      sol::returns_self()),
        "mul",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(const Fmatrix&, const Fmatrix&)>(&Fmatrix::mul), sol::resolve<Fmatrix&(const Fmatrix&, float)>(&Fmatrix::mul),
                                    sol::resolve<Fmatrix&(float)>(&Fmatrix::mul)),
                      sol::returns_self()),
        "invert", sol::policies(sol::overload(sol::resolve<Fmatrix&()>(&Fmatrix::invert), sol::resolve<Fmatrix&(const Fmatrix&)>(&Fmatrix::invert)), sol::returns_self()),
        "invert_b", sol::policies(&Fmatrix::invert_b, sol::returns_self()), "div",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(const Fmatrix&, float)>(&Fmatrix::div), sol::resolve<Fmatrix&(float)>(&Fmatrix::div)), sol::returns_self()), "scale",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(float, float, float)>(&Fmatrix::scale), sol::resolve<Fmatrix&(const Fvector&)>(&Fmatrix::scale)), sol::returns_self()),
        "setHPB",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(float, float, float)>(&Fmatrix::setHPB), sol::resolve<Fmatrix&(const Fvector&)>(&Fmatrix::setHPB)), sol::returns_self()),
        "setXYZ",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(float, float, float)>(&Fmatrix::setXYZ), sol::resolve<Fmatrix&(const Fvector&)>(&Fmatrix::setXYZ)), sol::returns_self()),
        "setXYZi",
        sol::policies(sol::overload(sol::resolve<Fmatrix&(float, float, float)>(&Fmatrix::setXYZi), sol::resolve<Fmatrix&(const Fvector&)>(&Fmatrix::setXYZi)),
                      sol::returns_self()),
        "getHPB", get_matrix_hpb, "getXYZi", get_matrix_xyzi);
}
