#pragma once

// maps unsigned 8 bits/channel to D3DCOLOR
[[nodiscard]] constexpr u32 color_argb(u32 a, u32 r, u32 g, u32 b) noexcept { return ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff); }
[[nodiscard]] constexpr u32 color_rgba(u32 r, u32 g, u32 b, u32 a) noexcept { return color_argb(a, r, g, b); }

[[nodiscard]] constexpr u32 color_argb_f(f32 a, f32 r, f32 g, f32 b) noexcept
{
    const auto _r{std::clamp(gsl::narrow_cast<u32>(std::floor(r * 255.0f)), 0u, 255u)};
    const auto _g{std::clamp(gsl::narrow_cast<u32>(std::floor(g * 255.0f)), 0u, 255u)};
    const auto _b{std::clamp(gsl::narrow_cast<u32>(std::floor(b * 255.0f)), 0u, 255u)};
    const auto _a{std::clamp(gsl::narrow_cast<u32>(std::floor(a * 255.0f)), 0u, 255u)};

    return color_argb(_a, _r, _g, _b);
}

[[nodiscard]] constexpr u32 color_rgba_f(f32 r, f32 g, f32 b, f32 a) noexcept { return color_argb_f(a, r, g, b); }
[[nodiscard]] constexpr u32 color_xrgb(u32 r, u32 g, u32 b) noexcept { return color_argb(0xff, r, g, b); }

[[nodiscard]] constexpr u32 color_get_R(u32 rgba) noexcept { return (((rgba) >> 16) & 0xff); }
[[nodiscard]] constexpr u32 color_get_G(u32 rgba) noexcept { return (((rgba) >> 8) & 0xff); }
[[nodiscard]] constexpr u32 color_get_B(u32 rgba) noexcept { return ((rgba) & 0xff); }
[[nodiscard]] constexpr u32 color_get_A(u32 rgba) noexcept { return ((rgba) >> 24); }

[[nodiscard]] constexpr u32 subst_alpha(u32 rgba, u32 a) noexcept { return (rgba & ~color_rgba(0, 0, 0, 0xff)) | color_rgba(0, 0, 0, a); }

[[nodiscard]] constexpr u32 bgr2rgb(u32 bgr) noexcept { return color_rgba(color_get_B(bgr), color_get_G(bgr), color_get_R(bgr), 0); }
[[nodiscard]] constexpr u32 rgb2bgr(u32 rgb) noexcept { return bgr2rgb(rgb); }

struct XR_TRIVIAL Fcolor
{
    union
    {
        struct
        {
            f32 r, g, b, a;
        };
        std::array<f32, 4> arr;
    };

    constexpr Fcolor() noexcept = default;
    constexpr explicit Fcolor(f32 _r, f32 _g, f32 _b, f32 _a) noexcept : r{_r}, g{_g}, b{_b}, a{_a} {}
    constexpr explicit Fcolor(u32 dw) noexcept { set(dw); }

    constexpr Fcolor(const Fcolor& that) noexcept { set(that); }
    constexpr Fcolor(Fcolor&&) noexcept = default;

    constexpr Fcolor& operator=(u32 dw) noexcept { return set(dw); }

    constexpr Fcolor& operator=(const Fcolor& that) noexcept { return set(that); }
    constexpr Fcolor& operator=(Fcolor&&) noexcept = default;

    constexpr Fcolor& set(u32 dw) noexcept
    {
        constexpr f32 f{1.0f / 255.0f};

        a = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw >> 24));
        b = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw >> 16));
        g = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw >> 8));
        r = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw));

        return *this;
    }

    constexpr Fcolor& set(f32 _r, f32 _g, f32 _b, f32 _a) noexcept
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;

        return *this;
    }

    constexpr Fcolor& set(const Fcolor& that) noexcept
    {
        arr = that.arr;
        return *this;
    }

    [[nodiscard]] constexpr u32 get() const noexcept { return color_rgba_f(r, g, b, a); }

    [[nodiscard]] constexpr u32 get_windows() const noexcept // Get color as a Windows DWORD value.
    {
        const auto _a{gsl::narrow_cast<u8>(a * 255.0f)};
        const auto _r{gsl::narrow_cast<u8>(r * 255.0f)};
        const auto _g{gsl::narrow_cast<u8>(g * 255.0f)};
        const auto _b{gsl::narrow_cast<u8>(b * 255.0f)};

        return (u32{_a} << 24) | (u32{_b} << 16) | (u32{_g} << 8) | _r;
    }

    constexpr Fcolor& set_windows(u32 dw) noexcept // Set color from a Windows DWORD color value.
    {
        constexpr f32 f{1.0f / 255.0f};

        a = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw >> 24));
        b = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw >> 16));
        g = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw >> 8));
        r = f * gsl::narrow_cast<f32>(gsl::narrow_cast<u8>(dw));

        return *this;
    }

    constexpr Fcolor& adjust_contrast(f32 f) noexcept // >1 - contrast will be increased
    {
        r = 0.5f + f * (r - 0.5f);
        g = 0.5f + f * (g - 0.5f);
        b = 0.5f + f * (b - 0.5f);
        return *this;
    }

    constexpr Fcolor& adjust_contrast(const Fcolor& in, f32 f) noexcept // >1 - contrast will be increased
    {
        r = 0.5f + f * (in.r - 0.5f);
        g = 0.5f + f * (in.g - 0.5f);
        b = 0.5f + f * (in.b - 0.5f);
        return *this;
    }

    constexpr Fcolor& adjust_saturation(f32 s) noexcept
    {
        // Approximate values for each component's contribution to luminance.
        // Based upon the NTSC standard described in ITU-R Recommendation BT.709.
        const f32 grey{r * 0.2125f + g * 0.7154f + b * 0.0721f};

        r = grey + s * (r - grey);
        g = grey + s * (g - grey);
        b = grey + s * (b - grey);

        return *this;
    }

    constexpr Fcolor& adjust_saturation(const Fcolor& in, f32 s) noexcept
    {
        // Approximate values for each component's contribution to luminance.
        // Based upon the NTSC standard described in ITU-R Recommendation BT.709.
        const f32 grey{in.r * 0.2125f + in.g * 0.7154f + in.b * 0.0721f};

        r = grey + s * (in.r - grey);
        g = grey + s * (in.g - grey);
        b = grey + s * (in.b - grey);

        return *this;
    }

    constexpr Fcolor& modulate(Fcolor& in) noexcept
    {
        r *= in.r;
        g *= in.g;
        b *= in.b;
        a *= in.a;

        return *this;
    }

    constexpr Fcolor& modulate(const Fcolor& in1, const Fcolor& in2) noexcept
    {
        r = in1.r * in2.r;
        g = in1.g * in2.g;
        b = in1.b * in2.b;
        a = in1.a * in2.a;

        return *this;
    }

    constexpr Fcolor& negative(const Fcolor& in) noexcept
    {
        r = 1.0f - in.r;
        g = 1.0f - in.g;
        b = 1.0f - in.b;
        a = 1.0f - in.a;

        return *this;
    }

    constexpr Fcolor& negative() noexcept
    {
        r = 1.0f - r;
        g = 1.0f - g;
        b = 1.0f - b;
        a = 1.0f - a;

        return *this;
    }

    constexpr Fcolor& sub_rgb(f32 s) noexcept
    {
        r -= s;
        g -= s;
        b -= s;

        return *this;
    }

    constexpr Fcolor& add_rgb(f32 s) noexcept
    {
        r += s;
        g += s;
        b += s;

        return *this;
    }

    constexpr Fcolor& add_rgba(f32 s) noexcept
    {
        r += s;
        g += s;
        b += s;
        a += s;

        return *this;
    }

    constexpr Fcolor& mul_rgba(f32 s) noexcept
    {
        r *= s;
        g *= s;
        b *= s;
        a *= s;

        return *this;
    }

    constexpr Fcolor& mul_rgb(f32 s) noexcept
    {
        r *= s;
        g *= s;
        b *= s;

        return *this;
    }

    constexpr Fcolor& mul_rgba(const Fcolor& c, f32 s) noexcept
    {
        r = c.r * s;
        g = c.g * s;
        b = c.b * s;
        a = c.a * s;

        return *this;
    }

    constexpr Fcolor& mul_rgb(const Fcolor& c, f32 s) noexcept
    {
        r = c.r * s;
        g = c.g * s;
        b = c.b * s;

        return *this;
    }

    // SQ magnitude
    [[nodiscard]] constexpr f32 magnitude_sqr_rgb() const noexcept { return r * r + g * g + b * b; }
    // magnitude
    [[nodiscard]] constexpr f32 magnitude_rgb() const noexcept { return _sqrt(magnitude_sqr_rgb()); }

    [[nodiscard]] constexpr f32 intensity() const noexcept
    {
        // XXX: Use the component percentages from adjust_saturation()?
        return (r + g + b) / 3.f;
    }

    // Normalize
    constexpr Fcolor& normalize_rgb() noexcept
    {
        VERIFY(magnitude_sqr_rgb() > EPS_S);

        return mul_rgb(1.f / magnitude_rgb());
    }

    constexpr Fcolor& normalize_rgb(const Fcolor& c) noexcept
    {
        VERIFY(c.magnitude_sqr_rgb() > EPS_S);

        return mul_rgb(c, 1.f / c.magnitude_rgb());
    }

    constexpr Fcolor& lerp(const Fcolor& c1, const Fcolor& c2, f32 t) noexcept
    {
        const f32 invt{1.f - t};

        r = c1.r * invt + c2.r * t;
        g = c1.g * invt + c2.g * t;
        b = c1.b * invt + c2.b * t;
        a = c1.a * invt + c2.a * t;

        return *this;
    }

    constexpr Fcolor& lerp(const Fcolor& c1, const Fcolor& c2, const Fcolor& c3, f32 t) noexcept
    {
        if (t > 0.5f)
            return lerp(c2, c3, t * 2.f - 1.f);
        else
            return lerp(c1, c2, t * 2.f);
    }

    [[nodiscard]] constexpr bool similar_rgba(const Fcolor& v, f32 E = EPS_L) const noexcept
    {
        return fsimilar(r, v.r, E) && fsimilar(g, v.g, E) && fsimilar(b, v.b, E) && fsimilar(a, v.a, E);
    }

    [[nodiscard]] constexpr bool similar_rgb(const Fcolor& v, f32 E = EPS_L) const noexcept { return fsimilar(r, v.r, E) && fsimilar(g, v.g, E) && fsimilar(b, v.b, E); }
};
XR_TRIVIAL_ASSERT(Fcolor);

[[nodiscard]] constexpr bool _valid(const Fcolor& c) noexcept { return _valid(c.r) && _valid(c.g) && _valid(c.b) && _valid(c.a); }
