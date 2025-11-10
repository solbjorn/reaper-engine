#ifndef vis_commonH
#define vis_commonH

#include "context.h"

struct XR_TRIVIAL alignas(32) vis_marker
{
private:
    std::array<ctx_id_t, R__NUM_CONTEXTS> ctx{};

public:
    constexpr vis_marker() = default;

    constexpr vis_marker(const vis_marker& that) { xr_memcpy32(this, &that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr vis_marker(vis_marker&&) = default;
#else
    constexpr vis_marker(vis_marker&& that) { xr_memcpy32(this, &that); }
#endif

    constexpr vis_marker& operator=(const vis_marker& that)
    {
        xr_memcpy32(this, &that);
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr vis_marker& operator=(vis_marker&&) = default;
#else
    constexpr vis_marker& operator=(vis_marker&& that)
    {
        xr_memcpy32(this, &that);
        return *this;
    }
#endif

    [[nodiscard]] constexpr ctx_id_t& operator[](gsl::index idx) { return ctx[gsl::narrow_cast<size_t>(idx)]; }
    [[nodiscard]] constexpr const ctx_id_t& operator[](gsl::index idx) const { return ctx[gsl::narrow_cast<size_t>(idx)]; }
};
static_assert(sizeof(vis_marker) == 32);
XR_TRIVIAL_ASSERT(vis_marker);

struct XR_TRIVIAL vis_data
{
    Fsphere sphere; //
    Fbox box; //

    u64 pad1;

    u32 accept_frame; // when it was requisted accepted for main render
    u32 hom_frame; // when to perform test - shedule
    u32 hom_tested; // when it was last time tested
    u32 pad2;

    vis_marker marker; // for different sub-renders

    constexpr vis_data() = default;

    constexpr vis_data(const vis_data& that) { xr_memcpy256(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr vis_data(vis_data&&) = default;
#else
    constexpr vis_data(vis_data&& that) { xr_memcpy256(this, &that, sizeof(that)); }
#endif

    constexpr vis_data& operator=(const vis_data& that)
    {
        xr_memcpy256(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr vis_data& operator=(vis_data&&) = default;
#else
    constexpr vis_data& operator=(vis_data&& that)
    {
        xr_memcpy256(this, &that, sizeof(that));
        return *this;
    }
#endif

    constexpr void clear()
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
XR_TRIVIAL_ASSERT(vis_data);

#endif
