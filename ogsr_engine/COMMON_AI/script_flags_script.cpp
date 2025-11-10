////////////////////////////////////////////////////////////////////////////
//	Module 		: script_flags_script.cpp
//	Created 	: 19.07.2004
//  Modified 	: 19.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script flags script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_flags.h"

namespace
{
template <typename T>
T& one(T& self)
{
    return self.assign(std::numeric_limits<typename T::TYPE>::max());
}

template <typename T>
T& set(T& self, typename T::TYPE mask, bool value)
{
    return self.set(mask, value);
}

template <typename T>
bool is(const T& self, typename T::TYPE mask)
{
    return !!self.is(mask);
}

template <typename T>
bool is_any(const T& self, typename T::TYPE mask)
{
    return !!self.is_any(mask);
}

template <typename T>
bool test(const T& self, typename T::TYPE mask)
{
    return !!self.test(mask);
}

template <typename T>
bool equal(const T& self, const T& f)
{
    return !!self.equal(f);
}

template <typename T>
bool equal(const T& self, const T& f, typename T::TYPE mask)
{
    return !!self.equal(f, mask);
}

template <typename T>
void add_flags(sol::state_view& lua, absl::string_view alias)
{
    lua.new_usertype<T>(alias, sol::no_constructor, sol::call_constructor, sol::constructors<T()>(), "get", &T::get, "zero", &T::zero, "one", &one<T>, "invert",
                        sol::overload(sol::resolve<T&()>(&T::invert), sol::resolve<T&(const T&)>(&T::invert), sol::resolve<T&(const typename T::TYPE)>(&T::invert)), "assign",
                        sol::overload(sol::resolve<T&(const T&)>(&T::assign), sol::resolve<T&(const typename T::TYPE)>(&T::assign)), "or",
                        sol::overload(sol::resolve<T&(const typename T::TYPE)>(&T::Or), sol::resolve<T&(const T&, const typename T::TYPE)>(&T::Or)), "and",
                        sol::overload(sol::resolve<T&(const typename T::TYPE)>(&T::And), sol::resolve<T&(const T&, const typename T::TYPE)>(&T::And)), "set", &set<T>, "is", &is<T>,
                        "is_any", &is_any<T>, "test", &test<T>, "equal",
                        sol::overload(sol::resolve<bool(const T&, const T&)>(&equal<T>), sol::resolve<bool(const T&, const T&, const typename T::TYPE)>(&equal<T>)));
}
} // namespace

template <>
void CScriptFlags::script_register(sol::state_view& lua)
{
    add_flags<Flags8>(lua, "flags8");
    add_flags<Flags16>(lua, "flags16");
    add_flags<Flags32>(lua, "flags32");
}
