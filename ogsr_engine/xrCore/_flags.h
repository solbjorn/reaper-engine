#pragma once

template <class T>
struct _flags
{
public:
    typedef T TYPE;
    typedef _flags<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;

public:
    T flags{};

    constexpr IC TYPE get() const { return flags; }

    constexpr IC SelfRef zero()
    {
        flags = T(0);
        return *this;
    }

    constexpr IC SelfRef one()
    {
        flags = T(-1);
        return *this;
    }

    constexpr IC SelfRef invert()
    {
        flags = ~flags;
        return *this;
    }

    constexpr IC SelfRef invert(const Self& f)
    {
        flags = ~f.flags;
        return *this;
    }

    constexpr IC SelfRef invert(const T mask)
    {
        flags ^= mask;
        return *this;
    }

    constexpr IC SelfRef assign(const Self& f)
    {
        flags = f.flags;
        return *this;
    }

    constexpr IC SelfRef assign(const T mask)
    {
        flags = mask;
        return *this;
    }

    constexpr IC SelfRef set(const T mask, BOOL value)
    {
        if (value)
            flags |= mask;
        else
            flags &= ~mask;
        return *this;
    }

    constexpr IC BOOL is(const T mask) const { return mask == (flags & mask); }
    constexpr IC BOOL is_any(const T mask) const { return BOOL(!!(flags & mask)); }
    constexpr IC BOOL test(const T mask) const { return BOOL(!!(flags & mask)); }

    constexpr IC SelfRef Or(const T mask)
    {
        flags |= mask;
        return *this;
    }

    constexpr IC SelfRef Or(const Self& f, const T mask)
    {
        flags = f.flags | mask;
        return *this;
    }

    constexpr IC SelfRef And(const T mask)
    {
        flags &= mask;
        return *this;
    }

    constexpr IC SelfRef And(const Self& f, const T mask)
    {
        flags = f.flags & mask;
        return *this;
    }

    constexpr IC BOOL equal(const Self& f) const { return flags == f.flags; }
    constexpr IC BOOL equal(const Self& f, const T mask) const { return (flags & mask) == (f.flags & mask); }
};

using Flags8 = _flags<u8>;
static_assert(sizeof(Flags8) == sizeof(u8));

using Flags16 = _flags<u16>;
static_assert(sizeof(Flags16) == sizeof(u16));

using Flags32 = _flags<u32>;
static_assert(sizeof(Flags32) == sizeof(u32));

using Flags64 = _flags<u64>;
static_assert(sizeof(Flags64) == sizeof(u64));
