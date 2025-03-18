#ifndef __XRCORE_BITMAP_H
#define __XRCORE_BITMAP_H

template <size_t N>
class xr_bitmap
{
public:
    constexpr ICF bool test(size_t pos) const { return !!(val & (1UL << pos)); }

    constexpr ICF void set(size_t pos) { val |= size_t(1) << pos; }
    constexpr ICF void clear(size_t pos) { val &= ~(size_t(1) << pos); }
    constexpr ICF void assign(size_t pos, bool en) { return en ? set(pos) : clear(pos); }

    constexpr ICF size_t ffz() const
    {
        size_t scan = ~(val | invmask);
        if (!scan)
            return len;

        unsigned long ret;
        _BitScanForward(&ret, scan);

        return ret;
    }

    constexpr ICF void zero() { val = 0; }

    constexpr ICF size_t size() const { return len; }

private:
    size_t val{};

    static constexpr size_t len = N;
    static_assert(len <= sizeof(size_t) * 8);

#define __GENMASK(h, l) (((~size_t(0)) - (size_t(1) << (l)) + 1) & (~size_t(0) >> (sizeof(size_t) * 8 - 1 - (h))))

    static constexpr size_t mask = __GENMASK(N - 1, 0);
    static constexpr size_t invmask = ~mask;

#undef __GENMASK
};
static_assert(sizeof(xr_bitmap<sizeof(size_t) * 8>) == sizeof(size_t));

#endif /* __XRCORE_BITMAP_H */
