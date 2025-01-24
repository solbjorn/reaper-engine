#ifndef __XRCORE_XRHASH_H
#define __XRCORE_XRHASH_H

#define RAPIDHASH_INLINE ICF
#include <rapidhash-msvc.h>

// Pack 64-bit value into <= 32 bits
// https://elixir.bootlin.com/linux/v6.13-rc3/source/include/linux/hash.h#L24
constexpr ICF u32 hash_64(const u64 val, const u32 bits)
{
#define GOLDEN_RATIO_64 0x61C8864680B583EBull
    return u32(val * GOLDEN_RATIO_64 >> (64 - bits));
}

// Templates for hashing basic types with RapidHash.
// Taken from https://github.com/martinus/unordered_dense replacing custom
// wyhash implementation with upstream RapidHash.

template <typename T, typename Enable = void>
struct xr_hash
{
    auto operator()(T const& obj) const noexcept(noexcept(std::declval<std::hash<T>>().operator()(std::declval<T const&>()))) -> u64 { return std::hash<T>{}(obj); }
};

template <typename T>
struct xr_hash<T, typename std::hash<T>::is_avalanching>
{
    using is_avalanching = void;
    auto operator()(T const& obj) const noexcept(noexcept(std::declval<std::hash<T>>().operator()(std::declval<T const&>()))) -> u64 { return std::hash<T>{}(obj); }
};

template <typename CharT>
struct xr_hash<std::basic_string<CharT>>
{
    using is_avalanching = void;
    auto operator()(std::basic_string<CharT> const& str) const noexcept -> u64 { return rapidhash_msvc(str.data(), sizeof(CharT) * str.size()); }
};

template <typename CharT>
struct xr_hash<std::basic_string_view<CharT>>
{
    using is_avalanching = void;
    auto operator()(std::basic_string_view<CharT> const& sv) const noexcept -> u64 { return rapidhash_msvc(sv.data(), sizeof(CharT) * sv.size()); }
};

template <class T>
struct xr_hash<T*>
{
    using is_avalanching = void;
    auto operator()(T* ptr) const noexcept -> u64 { return rapidhash_msvc(reinterpret_cast<uintptr_t>(ptr)); }
};

template <class T>
struct xr_hash<std::unique_ptr<T>>
{
    using is_avalanching = void;
    auto operator()(std::unique_ptr<T> const& ptr) const noexcept -> u64 { return rapidhash_msvc(reinterpret_cast<uintptr_t>(ptr.get())); }
};

template <class T>
struct xr_hash<std::shared_ptr<T>>
{
    using is_avalanching = void;
    auto operator()(std::shared_ptr<T> const& ptr) const noexcept -> u64 { return rapidhash_msvc(reinterpret_cast<uintptr_t>(ptr.get())); }
};

template <typename Enum>
struct xr_hash<Enum, typename std::enable_if<std::is_enum<Enum>::value>::type>
{
    using is_avalanching = void;
    auto operator()(Enum e) const noexcept -> u64
    {
        using underlying = typename std::underlying_type_t<Enum>;
        return rapidhash_msvc(static_cast<underlying>(e));
    }
};

template <typename... Args>
struct tuple_hash_helper
{
    template <typename Arg>
    [[nodiscard]] constexpr static auto to64(Arg const& arg) -> u64
    {
        if constexpr (std::is_integral_v<Arg> || std::is_enum_v<Arg>)
            return static_cast<u64>(arg);
        else
            return xr_hash<Arg>{}(arg);
    }

    [[nodiscard]] static auto mix64(u64 state, u64 v) -> u64 { return rapidhash_msvc(state + v); }

    template <typename T, std::size_t... Idx>
    [[nodiscard]] static auto calc_hash(T const& t, std::index_sequence<Idx...>) noexcept -> u64
    {
        auto h = u64{};

        ((h = mix64(h, to64(std::get<Idx>(t)))), ...);

        return h;
    }
};

template <typename... Args>
struct xr_hash<std::tuple<Args...>> : tuple_hash_helper<Args...>
{
    using is_avalanching = void;
    auto operator()(std::tuple<Args...> const& t) const noexcept -> u64 { return tuple_hash_helper<Args...>::calc_hash(t, std::index_sequence_for<Args...>{}); }
};

template <typename A, typename B>
struct xr_hash<std::pair<A, B>> : tuple_hash_helper<A, B>
{
    using is_avalanching = void;
    auto operator()(std::pair<A, B> const& t) const noexcept -> u64 { return tuple_hash_helper<A, B>::calc_hash(t, std::index_sequence_for<A, B>{}); }
};

#define XR_HASH_STATIC_CAST(T) \
    template <> \
    struct xr_hash<T> \
    { \
        using is_avalanching = void; \
        auto operator()(T const& obj) const noexcept -> u64 { return rapidhash_msvc(static_cast<u64>(obj)); } \
    }

XR_HASH_STATIC_CAST(bool);
XR_HASH_STATIC_CAST(char);
XR_HASH_STATIC_CAST(signed char);
XR_HASH_STATIC_CAST(unsigned char);
#if __cplusplus >= 202002L && defined(__cpp_char8_t)
XR_HASH_STATIC_CAST(char8_t);
#endif
XR_HASH_STATIC_CAST(char16_t);
XR_HASH_STATIC_CAST(char32_t);
XR_HASH_STATIC_CAST(wchar_t);
XR_HASH_STATIC_CAST(short);
XR_HASH_STATIC_CAST(unsigned short);
XR_HASH_STATIC_CAST(int);
XR_HASH_STATIC_CAST(unsigned int);
XR_HASH_STATIC_CAST(long);
XR_HASH_STATIC_CAST(long long);
XR_HASH_STATIC_CAST(unsigned long);
XR_HASH_STATIC_CAST(unsigned long long);

#undef XR_HASH_STATIC_CAST

#endif /* __XRCORE_XRHASH_H */
