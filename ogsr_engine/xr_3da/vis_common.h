#ifndef vis_commonH
#define vis_commonH

#include "context.h"

struct alignas(32) vis_marker
{
private:
    ctx_id_t ctx[R__NUM_CONTEXTS]{};

public:
    constexpr inline vis_marker() = default;
    constexpr inline vis_marker(const vis_marker& m) { xr_memcpy32(this, &m); }

    constexpr inline vis_marker& operator=(const vis_marker& m)
    {
        xr_memcpy32(this, &m);
        return *this;
    }

    constexpr inline ctx_id_t operator[](int idx) const { return ctx[idx]; }
    constexpr inline ctx_id_t& operator[](int idx) { return ctx[idx]; }
};
static_assert(sizeof(vis_marker) == 32);

struct vis_data
{
    Fsphere sphere; //
    Fbox box; //

    u64 pad1;

    u32 accept_frame; // when it was requisted accepted for main render
    u32 hom_frame; // when to perform test - shedule
    u32 hom_tested; // when it was last time tested
    u32 pad2;

    vis_marker marker; // for different sub-renders

    constexpr inline vis_data() = default;
    constexpr inline vis_data(const vis_data& v) { xr_memcpy256(this, &v, sizeof(v)); }

    constexpr inline vis_data& operator=(const vis_data& v)
    {
        xr_memcpy256(this, &v, sizeof(v));
        return *this;
    }

    IC void clear()
    {
        static constexpr ctx_id_t __declspec(align(32)) zeros[R__NUM_CONTEXTS]{};

        xr_memcpy16(&sphere, zeros);
        box.invalidate();

        xr_memcpy16(&accept_frame, zeros);
        xr_memcpy32(&marker, zeros);
    }
};
static_assert(offsetof(vis_data, accept_frame) == 48);
static_assert(offsetof(vis_data, pad2) + sizeof(u32) == 64);

#endif
