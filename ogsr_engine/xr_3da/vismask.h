#pragma once

struct VisMask final
{
    Flags64 _visimask;
    Flags64 _visimask_ex;

    constexpr VisMask()
    {
        _visimask.zero();
        _visimask_ex.zero();
    }

    constexpr VisMask(const VisMask& _second)
    {
        _visimask.flags = _second._visimask.flags;
        _visimask_ex.flags = _second._visimask_ex.flags;
    }

    constexpr explicit VisMask(u64 _low, u64 _high)
    {
        _visimask.assign(_low);
        _visimask_ex.assign(_high);
    }

    [[nodiscard]] constexpr bool operator!=(const VisMask& _second) const noexcept
    {
        if (_visimask.flags != _second._visimask.flags)
            return true;
        if (_visimask_ex.flags != _second._visimask_ex.flags)
            return true;
        return false;
    }

    [[nodiscard]] constexpr bool operator==(const VisMask& _second) const noexcept
    {
        return (_visimask.flags == _second._visimask.flags) && (_visimask_ex.flags == _second._visimask_ex.flags);
    }

    constexpr VisMask& operator=(const VisMask& _second)
    {
        _visimask.flags = _second._visimask.flags;
        _visimask_ex.flags = _second._visimask_ex.flags;
        return *this;
    }

    constexpr void set(u16 _digit, bool _set)
    {
        if (_digit < 64)
            _visimask.set(1ull << _digit, _set);
        else
            _visimask_ex.set(1ull << (_digit - 64), _set);
    }

    constexpr void set(u64 _low, u64 _high)
    {
        _visimask.assign(_low);
        _visimask_ex.assign(_high);
    }

    [[nodiscard]] constexpr bool is(u16 _digit) const
    {
        if (_digit < 64)
            return !!_visimask.is(1ull << _digit);
        else
            return !!_visimask_ex.is(1ull << (_digit - 64));
    }

    constexpr void zero()
    {
        _visimask.zero();
        _visimask_ex.zero();
    }

    [[nodiscard]] constexpr u16 count() const
    {
        u16 _c = 0;
        for (u16 i = 0; i < 64; ++i)
            if (_visimask.is(1ull << i))
                ++_c;
        for (u16 j = 0; j < 64; ++j)
            if (_visimask_ex.is(1ull << j))
                ++_c;
        return _c;
    }

    constexpr void set_all()
    {
        for (u16 i = 0; i < 128; ++i)
            this->set(i, true);
    }

    constexpr void And(const VisMask& _second)
    {
        _visimask.And(_second._visimask.flags);
        _visimask_ex.And(_second._visimask_ex.flags);
    }

    constexpr void invert()
    {
        _visimask.invert();
        _visimask_ex.invert();
    }
};
