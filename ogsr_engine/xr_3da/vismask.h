#ifndef __XRENGINE_VISMASK_H
#define __XRENGINE_VISMASK_H

struct VisMask final
{
private:
    xr::bitset<128> storage;

public:
    constexpr VisMask() noexcept = default;
    constexpr explicit VisMask(u64 low, u64 high) { set(low, high); }

    constexpr VisMask(const VisMask&) noexcept = default;
    constexpr VisMask(VisMask&&) noexcept = default;

    constexpr VisMask& operator=(const VisMask&) noexcept = default;
    constexpr VisMask& operator=(VisMask&&) noexcept = default;

    [[nodiscard]] constexpr bool operator==(const VisMask& that) const noexcept { return storage == that.storage; }
    [[nodiscard]] constexpr bool operator!=(const VisMask& that) const noexcept { return !(*this == that); }

    constexpr VisMask& set(gsl::index digit, bool set)
    {
        storage.set(gsl::narrow_cast<size_t>(digit), set);
        return *this;
    }

    constexpr VisMask& set(u64 low, u64 high)
    {
        const std::array<u64, 2> arr{low, high};

        for (auto [k, val] : xr::views_enumerate(arr))
        {
            auto bitset = val;

            while (bitset != 0)
            {
                const u64 t = bitset & -bitset;
                const auto r = std::countr_zero(bitset);

                storage.set(gsl::narrow_cast<size_t>(k * 64 + r));
                bitset ^= t;
            }
        }

        return *this;
    }

    [[nodiscard]] constexpr bool is(gsl::index digit) const { return storage.test(gsl::narrow_cast<size_t>(digit)); }

    constexpr VisMask& zero() noexcept
    {
        storage.reset();
        return *this;
    }

    [[nodiscard]] constexpr gsl::index count() const noexcept { return gsl::narrow_cast<gsl::index>(storage.count()); }

    constexpr VisMask& set_all() noexcept
    {
        storage.set();
        return *this;
    }

    constexpr VisMask& And(const VisMask& that) noexcept
    {
        storage &= that.storage;
        return *this;
    }

    constexpr VisMask& invert() noexcept
    {
        storage.flip();
        return *this;
    }

    [[nodiscard]] constexpr u64 to_u64(gsl::index part) const
    {
        const auto base = gsl::narrow_cast<size_t>(part * 64);
        R_ASSERT(base < storage.size());

        size_t index{base};
        u64 value = storage.test(index);

        while (true)
        {
            index = storage.next_one(index);
            if (index >= std::min(base + 64, storage.size()))
                break;

            value |= 1ull << (index - base);
        }

        return value;
    }
};

#endif // __XRENGINE_VISMASK_H
