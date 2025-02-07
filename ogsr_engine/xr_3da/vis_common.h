#ifndef vis_commonH
#define vis_commonH

struct vis_data
{
    Fsphere sphere; //
    Fbox box; //
    u32 marker; // for different sub-renders
    u32 accept_frame; // when it was requisted accepted for main render
    u32 hom_frame; // when to perform test - shedule
    u32 hom_tested; // when it was last time tested

    constexpr inline vis_data() = default;
    constexpr inline vis_data(const vis_data& v) { xr_memcpy128(this, &v, sizeof(v)); }

    constexpr inline vis_data& operator=(const vis_data& v)
    {
        xr_memcpy128(this, &v, sizeof(v));
        return *this;
    }

    IC void clear()
    {
        sphere.v.set(0, 0, 0, 0);
        box.invalidate();
        marker = 0;
        accept_frame = 0;
        hom_frame = 0;
        hom_tested = 0;
    }
};

#endif
