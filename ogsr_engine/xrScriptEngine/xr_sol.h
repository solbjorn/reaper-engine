#ifndef __XRSCRIPT_SOL_H
#define __XRSCRIPT_SOL_H

// Uncomment to turn on protected mode for every call and type conversion
#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>
#include <rtti.hh>

// Generate sol::bases<> and sol::base<T> using RTTI annotations

template <typename T>
using xr_sol_bases = typename RTTI::type_descriptor<T>::base_types ::template to<sol::bases>;

#define XR_SOL_BASE_CLASSES(T) \
    namespace sol \
    { \
    template <> \
    struct base<T> : std::true_type \
    { \
        typedef RTTI::type_descriptor<T>::declared_base_types ::template to<sol::types> type; \
    }; \
    } \
    static_assert(T::TypeInfo::Name() == RTTI::TypeName<T>())

// ^ Bonus assertion that T::TypeInfo is declared explicitly, not inherited

// Generate enum-style table with non-constexpr values (clsids, story IDs etc.)
template <bool read_only = true>
sol::table xr_sol_new_enum(sol::state_view& lua, absl::string_view name, sol::table& target)
{
    if constexpr (read_only)
    {
        sol::table x = lua.globals().create_with(sol::meta_function::new_index, sol::detail::fail_on_newindex, sol::meta_function::index, target, sol::meta_function::pairs,
                                                 sol::stack::stack_detail::readonly_pairs);
        sol::table shim = lua.globals().create_named(name, sol::metatable_key, x);

        return shim;
    }
    else
    {
        lua.globals().set(name, target);
        return target;
    }
}

// Essentially a copy of sol::usertype<T>::tuple_set() which is private, but needed for the function below
template <typename Class, std::size_t... I, typename... Args>
sol::usertype<Class>& xr_sol_tuple_set(sol::usertype<Class>& ut, std::index_sequence<I...>, std::tuple<Args...>&& args)
{
    (void)sol::detail::swallow{0, (ut.set(std::get<I * 2>(std::move(args)), std::get<I * 2 + 1>(std::move(args))), 0)...};
    return ut;
}

// sol::table::set() can take varargs; however, sol::usertype<T>::set() can not.
// Fix this using the same semantics as in the vararg sol::table::new_usertype<T>().
template <typename Class, typename... Args>
sol::usertype<Class>& xr_sol_set(sol::usertype<Class>& ut, Args&&... args)
{
    static_assert(!(sizeof...(Args) % 2));
    return xr_sol_tuple_set(ut, std::make_index_sequence<(sizeof...(Args)) / 2>(), std::forward_as_tuple(std::forward<Args>(args)...));
}

#endif /* __XRSCRIPT_SOL_H */
