////////////////////////////////////////////////////////////////////////////
//	Module 		: script_fvector_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float vector script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_fvector.h"

namespace
{
constexpr std::tuple<Fvector, Fvector> generate_orthonormal_basis(const Fvector& self)
{
    std::tuple<Fvector, Fvector> upright;

    Fvector::generate_orthonormal_basis(self, std::get<0>(upright), std::get<1>(upright));

    return upright;
}

constexpr std::tuple<Fvector, Fvector> generate_orthonormal_basis_normalized(Fvector& self)
{
    std::tuple<Fvector, Fvector> upright;

    Fvector::generate_orthonormal_basis_normalized(self, std::get<0>(upright), std::get<1>(upright));

    return upright;
}
} // namespace

template <>
void CScriptFvector::script_register(sol::state_view& lua)
{
    lua.new_usertype<Fvector>(
        "vector", sol::no_constructor, sol::call_constructor,
        sol::initializers([](Fvector& v) { v = Fvector{}; }, [](Fvector& v, const Fvector& other) { v = other; },
                          [](Fvector& v, float x, float y, float z) { v = Fvector(x, y, z); }),
        "x", &Fvector::x, "y", &Fvector::y, "z", &Fvector::z, "set",
        sol::policies(sol::overload(sol::resolve<Fvector&(float, float, float)>(&Fvector::set), sol::resolve<Fvector&(const Fvector&)>(&Fvector::set)), sol::returns_self()), "add",
        sol::policies(sol::overload(sol::resolve<Fvector&(float)>(&Fvector::add), sol::resolve<Fvector&(const Fvector&)>(&Fvector::add),
                                    sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::add), sol::resolve<Fvector&(const Fvector&, float)>(&Fvector::add)),
                      sol::returns_self()),
        "sub",
        sol::policies(sol::overload(sol::resolve<Fvector&(float)>(&Fvector::sub), sol::resolve<Fvector&(const Fvector&)>(&Fvector::sub),
                                    sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::sub), sol::resolve<Fvector&(const Fvector&, float)>(&Fvector::sub)),
                      sol::returns_self()),
        "mul",
        sol::policies(sol::overload(sol::resolve<Fvector&(float)>(&Fvector::mul), sol::resolve<Fvector&(const Fvector&)>(&Fvector::mul),
                                    sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::mul), sol::resolve<Fvector&(const Fvector&, float)>(&Fvector::mul)),
                      sol::returns_self()),
        "div",
        sol::policies(sol::overload(sol::resolve<Fvector&(float)>(&Fvector::div), sol::resolve<Fvector&(const Fvector&)>(&Fvector::div),
                                    sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::div), sol::resolve<Fvector&(const Fvector&, float)>(&Fvector::div)),
                      sol::returns_self()),
        "invert", sol::policies(sol::overload(sol::resolve<Fvector&()>(&Fvector::invert), sol::resolve<Fvector&(const Fvector&)>(&Fvector::invert)), sol::returns_self()), "min",
        sol::policies(sol::overload(sol::resolve<Fvector&(const Fvector&)>(&Fvector::min), sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::min)),
                      sol::returns_self()),
        "max",
        sol::policies(sol::overload(sol::resolve<Fvector&(const Fvector&)>(&Fvector::max), sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::max)),
                      sol::returns_self()),
        "abs", sol::policies(&Fvector::abs, sol::returns_self()), "similar", &Fvector::similar, "set_length", sol::policies(&Fvector::set_length, sol::returns_self()), "align",
        sol::policies(&Fvector::align, sol::returns_self()), "clamp",
        sol::policies(sol::overload(sol::resolve<Fvector&(const Fvector&)>(&Fvector::clamp), sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::clamp)),
                      sol::returns_self()),
        "inertion", sol::policies(&Fvector::inertion, sol::returns_self()), "average",
        sol::policies(sol::overload(sol::resolve<Fvector&(const Fvector&)>(&Fvector::average), sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::average)),
                      sol::returns_self()),
        "lerp", sol::policies(&Fvector::lerp, sol::returns_self()), "mad",
        sol::policies(sol::overload(sol::resolve<Fvector&(const Fvector&, float)>(&Fvector::mad), sol::resolve<Fvector&(const Fvector&, const Fvector&, float)>(&Fvector::mad),
                                    sol::resolve<Fvector&(const Fvector&, const Fvector&)>(&Fvector::mad),
                                    sol::resolve<Fvector&(const Fvector&, const Fvector&, const Fvector&)>(&Fvector::mad)),
                      sol::returns_self()),
        "magnitude", &Fvector::magnitude, "normalize",
        sol::policies(sol::overload(sol::resolve<Fvector&()>(&Fvector::normalize_safe), sol::resolve<Fvector&(const Fvector&)>(&Fvector::normalize_safe)), sol::returns_self()),
        "normalize_safe",
        sol::policies(sol::overload(sol::resolve<Fvector&()>(&Fvector::normalize_safe), sol::resolve<Fvector&(const Fvector&)>(&Fvector::normalize_safe)), sol::returns_self()),
        "dotproduct", &Fvector::dotproduct, "crossproduct", sol::policies(&Fvector::crossproduct, sol::returns_self()), "distance_to_xz", &Fvector::distance_to_xz,
        "distance_to_sqr", &Fvector::distance_to_sqr, "distance_to", &Fvector::distance_to, "setHP", sol::policies(&Fvector::setHP, sol::returns_self()), "getH", &Fvector::getH,
        "getP", &Fvector::getP, "reflect", sol::policies(&Fvector::reflect, sol::returns_self()), "slide", sol::policies(&Fvector::slide, sol::returns_self()),
        "generate_orthonormal_basis", &generate_orthonormal_basis, "generate_orthonormal_basis_normalized", &generate_orthonormal_basis_normalized);

    lua.new_usertype<Fvector2>(
        "vector2", sol::no_constructor, sol::call_constructor,
        sol::initializers([](Fvector2& v) { v = Fvector2{}; }, [](Fvector2& v, const Fvector2& other) { v = other; }, [](Fvector2& v, float x, float y) { v = Fvector2(x, y); }),
        "x", &Fvector2::x, "y", &Fvector2::y, "set",
        sol::policies(sol::overload(sol::resolve<Fvector2&(float, float)>(&Fvector2::set), sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::set)), sol::returns_self()), "add",
        sol::policies(sol::overload(sol::resolve<Fvector2&(float)>(&Fvector2::add), sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::add),
                                    sol::resolve<Fvector2&(const Fvector2&, const Fvector2&)>(&Fvector2::add), sol::resolve<Fvector2&(const Fvector2&, float)>(&Fvector2::add)),
                      sol::returns_self()),
        "sub",
        sol::policies(sol::overload(sol::resolve<Fvector2&(float)>(&Fvector2::sub), sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::sub),
                                    sol::resolve<Fvector2&(const Fvector2&, const Fvector2&)>(&Fvector2::sub), sol::resolve<Fvector2&(const Fvector2&, float)>(&Fvector2::sub)),
                      sol::returns_self()),
        "mul", sol::policies(sol::overload(sol::resolve<Fvector2&(float)>(&Fvector2::mul), sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::mul)), sol::returns_self()), "div",
        sol::policies(sol::overload(sol::resolve<Fvector2&(float)>(&Fvector2::div), sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::div)), sol::returns_self()), "min",
        sol::policies(sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::min), sol::returns_self()), "max",
        sol::policies(sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::max), sol::returns_self()), "abs", sol::policies(&Fvector2::abs, sol::returns_self()), "mad",
        sol::policies(&Fvector2::mad, sol::returns_self()), "magnitude", &Fvector2::magnitude, "normalize",
        sol::policies(sol::overload(sol::resolve<Fvector2&()>(&Fvector2::normalize_safe), sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::normalize_safe)),
                      sol::returns_self()),
        "normalize_safe",
        sol::policies(sol::overload(sol::resolve<Fvector2&()>(&Fvector2::normalize_safe), sol::resolve<Fvector2&(const Fvector2&)>(&Fvector2::normalize_safe)),
                      sol::returns_self()),
        "dotproduct", &Fvector2::dotproduct, "crossproduct", sol::policies(&Fvector2::crossproduct, sol::returns_self()), "distance_to", &Fvector2::distance_to, "getH",
        &Fvector2::getH);

    lua.new_usertype<Fvector4>(
        "vector4", sol::no_constructor, sol::call_constructor,
        sol::initializers([](Fvector4& v) { v = Fvector4{}; }, [](Fvector4& v, const Fvector4& other) { v = other; },
                          [](Fvector4& v, float x, float y, float z, float w) { v = Fvector4(x, y, z, w); }),
        "x", sol::property(&Fvector4::getx, &Fvector4::setx), "y", sol::property(&Fvector4::gety, &Fvector4::sety), "z", sol::property(&Fvector4::getz, &Fvector4::setz), "w",
        sol::property(&Fvector4::getw, &Fvector4::setw), "set",
        sol::policies(sol::overload(sol::resolve<Fvector4&(float, float, float, float)>(&Fvector4::set), sol::resolve<Fvector4&(const Fvector4&)>(&Fvector4::set)),
                      sol::returns_self()),
        "add",
        sol::policies(sol::overload(sol::resolve<Fvector4&(float)>(&Fvector4::add), sol::resolve<Fvector4&(const Fvector4&)>(&Fvector4::add),
                                    sol::resolve<Fvector4&(const Fvector4&, const Fvector4&)>(&Fvector4::add), sol::resolve<Fvector4&(const Fvector4&, float)>(&Fvector4::add)),
                      sol::returns_self()),
        "sub",
        sol::policies(sol::overload(sol::resolve<Fvector4&(float)>(&Fvector4::sub), sol::resolve<Fvector4&(const Fvector4&)>(&Fvector4::sub),
                                    sol::resolve<Fvector4&(const Fvector4&, const Fvector4&)>(&Fvector4::sub), sol::resolve<Fvector4&(const Fvector4&, float)>(&Fvector4::sub)),
                      sol::returns_self()),
        "mul", sol::policies(sol::overload(sol::resolve<Fvector4&(float)>(&Fvector4::mul), sol::resolve<Fvector4&(const Fvector4&)>(&Fvector4::mul)), sol::returns_self()), "div",
        sol::policies(sol::overload(sol::resolve<Fvector4&(float)>(&Fvector4::div), sol::resolve<Fvector4&(const Fvector4&)>(&Fvector4::div)), sol::returns_self()), "magnitude",
        &Fvector4::magnitude, "normalize", sol::policies(&Fvector4::normalize, sol::returns_self()));

    lua.new_usertype<Fbox>("Fbox", sol::no_constructor, sol::call_constructor, sol::initializers([](Fbox& b) { b = Fbox{}; }), "min", &Fbox::min, "max", &Fbox::max);

    lua.new_usertype<Frect>("Frect", sol::no_constructor, sol::call_constructor, sol::initializers([](Frect& r) { r = Frect{}; }), "set",
                            sol::policies(sol::resolve<Frect&(float, float, float, float)>(&Frect::set), sol::returns_self()), "lt", &Frect::lt, "rb", &Frect::rb, "x1", &Frect::x1,
                            "x2", &Frect::x2, "y1", &Frect::y1, "y2", &Frect::y2);
}
