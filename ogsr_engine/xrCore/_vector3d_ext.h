#pragma once

#include "_vector3d.h"

[[nodiscard]] constexpr Fvector cr_fvector3(f32 f) { return Fvector{f, f, f}; }

[[nodiscard]] constexpr Fvector cr_fvector3(f32 x, f32 y, f32 z) { return Fvector{x, y, z}; }

[[nodiscard]] constexpr Fvector cr_fvector3_hp(f32 h, f32 p) { return Fvector{}.setHP(h, p); }

constexpr Fvector operator+(const Fvector& v1, const Fvector& v2) { return cr_fvector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
constexpr Fvector operator-(const Fvector& v1, const Fvector& v2) { return cr_fvector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
constexpr Fvector operator-(const Fvector& v) { return cr_fvector3(-v.x, -v.y, -v.z); }

constexpr Fvector operator*(const Fvector& v, f32 f) { return cr_fvector3(v.x * f, v.y * f, v.z * f); }
constexpr Fvector operator*(f32 f, const Fvector& v) { return cr_fvector3(v.x * f, v.y * f, v.z * f); }

constexpr Fvector operator/(const Fvector& v, f32 f)
{
    const f32 repr_f = 1.f / f;
    return cr_fvector3(v.x * repr_f, v.y * repr_f, v.z * repr_f);
}

constexpr Fvector _min(const Fvector& v1, const Fvector& v2)
{
    Fvector r;
    r.min(v1, v2);
    return r;
}

constexpr Fvector _max(const Fvector& v1, const Fvector& v2)
{
    Fvector r;
    r.max(v1, v2);
    return r;
}

constexpr Fvector _abs(const Fvector& v)
{
    Fvector r;
    r.abs(v);
    return r;
}

constexpr Fvector normalize(const Fvector& v)
{
    Fvector r(v);
    r.normalize();
    return r;
}

[[nodiscard]] constexpr f32 magnitude(const Fvector& v) { return v.magnitude(); }
[[nodiscard]] constexpr f32 sqaure_magnitude(const Fvector& v) { return v.square_magnitude(); }
[[nodiscard]] constexpr f32 dotproduct(const Fvector& v1, const Fvector& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// CrossProduct
constexpr Fvector crossproduct(const Fvector& v1, const Fvector& v2)
{
    Fvector r;
    r.crossproduct(v1, v2);
    return r;
}

constexpr Fvector cr_vectorHP(f32 h, f32 p)
{
    f32 ch = _cos(h), cp = _cos(p), sh = _sin(h), sp = _sin(p);
    Fvector r;
    r.x = -cp * sh;
    r.y = sp;
    r.z = cp * ch;
    return r;
}

[[nodiscard]] constexpr f32 angle_between_vectors(const Fvector& v1, const Fvector& v2)
{
    const f32 mag1 = v1.magnitude();
    const f32 mag2 = v2.magnitude();
    constexpr f32 epsilon{1e-6f};

    if (mag1 < epsilon || mag2 < epsilon)
        return 0.0f;

    f32 angle_cos = dotproduct(v1, v2) / (mag1 * mag2);
    angle_cos = std::clamp(angle_cos, -1.0f, 1.0f);

    return std::acos(angle_cos);
}

constexpr Fvector rotate_point(const Fvector& point, f32 angle)
{
    f32 sin_alpha;
    f32 cos_alpha;
    DirectX::XMScalarSinCos(&sin_alpha, &cos_alpha, angle);

    return Fvector{point.x * cos_alpha - point.z * sin_alpha, 0, point.x * sin_alpha + point.z * cos_alpha};
}
