#ifndef __XR_SOL_H
#define __XR_SOL_H

// Enable checks for types and arguments
#define SOL_ALL_SAFETIES_ON 1
// Don't allow any script errors
#define SOL_SAFE_FUNCTIONS 0

// Don't rely on LuaJIT's non-conformant exceptions
#define SOL_EXCEPTIONS_ALWAYS_UNSAFE 1
#define SOL_EXCEPTIONS_CATCH_ALL 1

// Override poor compiler auto-detection
#define SOL_COMPILER_CLANG 1
#define SOL_COMPILER_VCXX 0

// Re-enable RTTI (gets disabled by `VCXX 0`)
#ifndef _CPPRTTI
#define SOL_NO_RTTI 1
#else
#define SOL_NO_RTTI 0
#endif

// Isn't defined by default for some reason
#define SOL_HEADER_ONLY 1

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wcast-qual");
XR_DIAG_IGNORE("-Wcharacter-conversion");
XR_DIAG_IGNORE("-Wcomma");
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wimplicit-fallthrough");
XR_DIAG_IGNORE("-Wnewline-eof");
XR_DIAG_IGNORE("-Wnrvo");
XR_DIAG_IGNORE("-Wundefined-reinterpret-cast");
XR_DIAG_IGNORE("-Wunused-template");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <sol/sol.hpp>

XR_DIAG_POP();

#include <rtti.hh>

namespace xr
{
// Generate sol::bases<> and sol::base<T> using RTTI annotations

template <typename T>
using sol_bases = typename RTTI::type_descriptor<T>::base_types ::template to<sol::bases>;

#define XR_SOL_BASE_CLASSES(T) \
    template <> \
    struct sol::base<T> : std::true_type \
    { \
        typedef RTTI::type_descriptor<T>::declared_base_types ::template to<sol::types> type; \
    }; \
    static_assert(T::TypeInfo::Name() == RTTI::TypeName<T>())

// ^ Bonus assertion that T::TypeInfo is declared explicitly, not inherited

// Generate enum-style table with non-constexpr values (clsids, story IDs etc.)
template <bool read_only = true>
XR_SYSV inline sol::table sol_new_enum(sol::state_view& lua, absl::string_view name, sol::table& target)
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
inline sol::usertype<Class>& sol_tuple_set(sol::usertype<Class>& ut, std::index_sequence<I...>, std::tuple<Args...>&& args)
{
    std::ignore = sol::detail::swallow{0, (ut.set(std::get<I * 2>(std::move(args)), std::get<I * 2 + 1>(std::move(args))), 0)...};
    return ut;
}

// sol::table::set() can take varargs; however, sol::usertype<T>::set() can not.
// Fix this using the same semantics as in the vararg sol::table::new_usertype<T>().
template <typename Class, typename... Args>
inline sol::usertype<Class>& sol_set(sol::usertype<Class>& ut, Args&&... args)
{
    static_assert(!(sizeof...(Args) % 2));
    return sol_tuple_set(ut, std::make_index_sequence<(sizeof...(Args)) / 2>(), std::forward_as_tuple(std::forward<Args>(args)...));
}
} // namespace xr

#endif // __XR_SOL_H
