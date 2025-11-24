#pragma once

// #define USE_MEMORY_VALIDATOR

#ifdef USE_MEMORY_VALIDATOR

#include <source_location>

#include "xrMemoryDebug.h"

extern BOOL g_enable_memory_debug;

#endif

#if _MSC_VER >= 1900 && !defined(__EDG__)
#define XR_RESTRICT __declspec(allocator) __declspec(restrict)
#else
#define XR_RESTRICT __declspec(restrict)
#endif

class xrMemory
{
public:
    [[nodiscard]] static gsl::index mem_usage(gsl::index* pBlocksUsed = nullptr, gsl::index* pBlocksFree = nullptr);
    static void mem_compact();

    [[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT static void* mem_alloc_aligned(gsl::index size, gsl::index align) noexcept;
    [[gnu::returns_nonnull]] [[nodiscard]] static void* mem_realloc_aligned(void* p, gsl::index size, gsl::index align) noexcept;
    static void mem_free_aligned(void* p) noexcept;
};

const inline xrMemory Memory;

namespace xr
{
constexpr inline gsl::index default_align_v = std::max(16z, gsl::index{__STDCPP_DEFAULT_NEW_ALIGNMENT__});

template <typename T>
constexpr inline gsl::index align_v = std::max(gsl::index{alignof(T)}, xr::default_align_v);

#define XR_ALIGNED_DEFAULT __declspec(align(xr::default_align_v))
} // namespace xr

#ifdef USE_MEMORY_VALIDATOR

[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT inline void* xr_malloc(gsl::index size, const std::source_location& loc = std::source_location::current())
{
    void* ptr = Memory.mem_alloc_aligned(size, xr::default_align_v);

    if (g_enable_memory_debug)
    {
        std::string id = std::string{"raw: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name();
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), false, false, size});
    }

    return ptr;
}

[[gnu::returns_nonnull]] [[nodiscard]] inline void* xr_realloc(void* p, gsl::index size, const std::source_location& loc = std::source_location::current())
{
    PointerRegistryRelease(p, loc);
    void* ptr = Memory.mem_realloc_aligned(p, size, xr::default_align_v);

    if (g_enable_memory_debug)
    {
        std::string id = std::string{"raw: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name();
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), false, false, size});
    }

    return ptr;
}

inline void xr_mfree(void* p, const std::source_location& loc = std::source_location::current())
{
    PointerRegistryRelease(p, loc);
    Memory.mem_free_aligned(p);
}

// generic "C"-like allocations/deallocations
template <typename T>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT inline T* xr_alloc(gsl::index count, const bool is_container = false,
                                                                      const std::source_location& loc = std::source_location::current())
{
    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(count * gsl::index{sizeof(T)}, xr::align_v<T>));

    if (g_enable_memory_debug)
    {
        std::string id = std::string{is_container ? "xr_allocator: " : "xr_alloc: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name() + " (" +
            typeid(T).name() + ")";
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), false, is_container, count * sizeof(T)});
    }

    return ptr;
}

template <typename T>
inline void xr_free(T*& P, const std::source_location& loc = std::source_location::current())
{
    if (P)
    {
        PointerRegistryRelease(P, loc);
        Memory.mem_free_aligned(P);

        P = nullptr;
    }
}

template <typename T>
void registerClass(T* ptr, const std::source_location& loc)
{
    if (g_enable_memory_debug)
    {
        std::string id = std::string{"xr_new: "} + loc.file_name() + "." + std::to_string(loc.line()) + ": " + loc.function_name() + " (" + typeid(T).name() + ")";
        PointerRegistryAdd(ptr, {std::move(id), loc.line(), true, false, sizeof(T)});
    }
}

template <typename T>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(const std::source_location& loc = std::source_location::current())
{
    if (std::is_constant_evaluated())
        return new T{};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    registerClass(ptr, loc);

    return new (ptr) T{};
}

template <typename T, typename A0>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(A0&& a0, const std::source_location& loc = std::source_location::current())
{
    if (std::is_constant_evaluated())
        return new T{std::forward<A0>(a0)};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    registerClass(ptr, loc);

    return new (ptr) T{std::forward<A0>(a0)};
}

template <typename T, typename A0, typename A1>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(A0&& a0, A1&& a1, const std::source_location& loc = std::source_location::current())
{
    if (std::is_constant_evaluated())
        return new T{std::forward<A0>(a0), std::forward<A1>(a1)};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    registerClass(ptr, loc);

    return new (ptr) T{std::forward<A0>(a0), std::forward<A1>(a1)};
}

template <typename T, typename A0, typename A1, typename A2>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(A0&& a0, A1&& a1, A2&& a2, const std::source_location& loc = std::source_location::current())
{
    if (std::is_constant_evaluated())
        return new T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2)};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    registerClass(ptr, loc);

    return new (ptr) T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2)};
}

template <typename T, typename A0, typename A1, typename A2, typename A3>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(A0&& a0, A1&& a1, A2&& a2, A3&& a3, const std::source_location& loc = std::source_location::current())
{
    if (std::is_constant_evaluated())
        return new T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3)};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    registerClass(ptr, loc);

    return new (ptr) T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3)};
}

template <typename T, typename A0, typename A1, typename A2, typename A3, typename A4>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(A0&& a0, A1&& a1, A2&& a2, A3&& a3, A4&& a4,
                                                                       const std::source_location& loc = std::source_location::current())
{
    if (std::is_constant_evaluated())
        return new T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4)};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    registerClass(ptr, loc);

    return new (ptr) T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4)};
}

template <typename T, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(A0&& a0, A1&& a1, A2&& a2, A3&& a3, A4&& a4, A5&& a5,
                                                                       const std::source_location& loc = std::source_location::current())
{
    if (std::is_constant_evaluated())
        return new T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4), std::forward<A5>(a5)};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    registerClass(ptr, loc);

    return new (ptr) T{std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4), std::forward<A5>(a5)};
}

template <typename T>
constexpr void xr_delete(T*& ptr, const std::source_location& loc = std::source_location::current())
{
    if (ptr)
    {
        if (std::is_constant_evaluated())
        {
            delete ptr;
        }
        else if constexpr (std::is_polymorphic_v<T>)
        {
            void* _real_ptr = smart_cast<void*>(ptr);

            ptr->~T();
            PointerRegistryRelease(_real_ptr, loc, true);
            Memory.mem_free_aligned(_real_ptr);
        }
        else
        {
            ptr->~T();
            PointerRegistryRelease(ptr, loc, true);
            Memory.mem_free_aligned(ptr);
        }

        ptr = nullptr;
    }
}

#else // !USE_MEMORY_VALIDATOR

#define xr_malloc(size) Memory.mem_alloc_aligned(size, xr::default_align_v)
#define xr_realloc(p, size) Memory.mem_realloc_aligned(p, size, xr::default_align_v)
#define xr_mfree(p) Memory.mem_free_aligned(p)

// generic "C"-like allocations/deallocations
template <typename T>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT inline T* xr_alloc(gsl::index count) noexcept
{
    return static_cast<T*>(Memory.mem_alloc_aligned(count * gsl::index{sizeof(T)}, xr::align_v<T>));
}

template <typename T>
inline void xr_free(T*& P) noexcept
{
    if (P)
    {
        Memory.mem_free_aligned(P);
        P = nullptr;
    }
}

template <typename T, typename... Args>
[[gnu::returns_nonnull]] [[nodiscard]] XR_RESTRICT constexpr T* xr_new(Args&&... args)
{
    if (std::is_constant_evaluated())
        return new T{std::forward<Args>(args)...};

    T* ptr = static_cast<T*>(Memory.mem_alloc_aligned(sizeof(T), xr::align_v<T>));
    return new (ptr) T{std::forward<Args>(args)...};
}

template <typename T>
constexpr void xr_delete(T*& ptr)
{
    if (ptr)
    {
        if (std::is_constant_evaluated())
        {
            delete ptr;
        }
        else if constexpr (std::is_polymorphic_v<T>)
        {
            void* _real_ptr = smart_cast<void*>(ptr);

            ptr->~T();
            Memory.mem_free_aligned(_real_ptr);
        }
        else
        {
            ptr->~T();
            Memory.mem_free_aligned(ptr);
        }

        ptr = nullptr;
    }
}

#endif // !USE_MEMORY_VALIDATOR

[[nodiscard]] gsl::index mem_usage_impl(gsl::index* pBlocksUsed, gsl::index* pBlocksFree);

struct SProcessMemInfo
{
    u64 PeakWorkingSetSize;
    u64 WorkingSetSize;

    u64 PeakPagefileUsage;
    u64 PagefileUsage;

    u64 TotalPhysicalMemory;
    u64 FreePhysicalMemory;
    u64 TotalPageFile;
    u64 FreePageFile;

    u32 MemoryLoad;
};

void GetProcessMemInfo(SProcessMemInfo& minfo);
