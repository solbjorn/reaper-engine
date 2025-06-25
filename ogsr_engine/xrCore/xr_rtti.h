#ifndef __XRCORE_RTTI_H
#define __XRCORE_RTTI_H

// Implementation of {dynamic,smart}_cast() using LLVM-style Open Hierarchy RTTI,
// including <void*> cast for operator delete().

#include <rtti.hh>

// External exceptions. Classes coming from 3rd party headers can be handled
// only using standard dynamic_cast().

namespace Opcode
{
class AABBNoLeafTree;
class Model;
}; // namespace Opcode

template <typename T>
concept class_exists = requires(T const* ptr)
{
    sizeof(*ptr) > 0;
};

// Uncomment to enable runtime checks for the RTTI tree validity. Each result
// of smart_cast() will be rechecked against the result of dynamic_cast() to
// make sure every class has its own RTTI, not inherited from a parent class.
// #define XR_RTTI_DEBUG

template <typename To, typename From>
[[nodiscard]] To smart_cast(From* from) noexcept
{
    if (!from)
        return nullptr;

    using Target = std::conditional_t<std::is_const_v<From>, const To, To>;

    if constexpr ((class_exists<Opcode::AABBNoLeafTree> && std::is_same_v<From, Opcode::AABBNoLeafTree>) || (class_exists<Opcode::Model> && std::is_same_v<From, Opcode::Model>))
        return dynamic_cast<Target>(from);
    else if constexpr (std::is_base_of_v<std::remove_pointer_t<Target>, From> && std::is_nothrow_convertible_v<From*, Target>)
#ifdef XR_RTTI_DEBUG
    {
        auto ret = static_cast<Target>(from);
        R_ASSERT(ret == dynamic_cast<To>(from));

        return ret;
    }
#else
        return static_cast<Target>(from);
#endif
    else
#ifdef XR_RTTI_DEBUG
    {
        auto ret = from->cast<std::remove_pointer_t<Target>>();
        R_ASSERT(ret == dynamic_cast<To>(from));

        return ret;
    }
#else
        return from->cast<std::remove_pointer_t<Target>>();
#endif
}

#endif /* __XRCORE_RTTI_H */
