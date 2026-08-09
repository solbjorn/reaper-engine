#ifndef __XREXTERNAL_RTTI_H
#define __XREXTERNAL_RTTI_H

#include "../xrCore/defines.h"

#include "assert.h"

// Implementation of {dynamic,smart}_cast() using LLVM-style Open Hierarchy RTTI,
// including <void*> cast for operator delete().

#include <rtti.hh>

template <typename To, typename From>
[[nodiscard]] To smart_cast(From* from) noexcept
{
    if (from == nullptr)
        return nullptr;

    using Target = std::conditional_t<std::is_const_v<From>, const To, To>;

    if constexpr (std::is_base_of_v<std::remove_pointer_t<Target>, From> && std::is_nothrow_convertible_v<From*, Target>)
#ifdef XR_RTTI_DEBUG
        return LIBASSERT_ASSERT_VAL(static_cast<Target>(from) == dynamic_cast<To>(from));
#else
        return static_cast<Target>(from);
#endif
    else
#ifdef XR_RTTI_DEBUG
        return LIBASSERT_ASSERT_VAL(from->template cast<std::remove_pointer_t<Target>>() == dynamic_cast<To>(from));
#else
        return from->template cast<std::remove_pointer_t<Target>>();
#endif
}

#endif // !__XREXTERNAL_RTTI_H
