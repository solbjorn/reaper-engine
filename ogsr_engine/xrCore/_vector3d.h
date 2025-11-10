#ifndef __XR_CORE_VECTOR3D_H
#define __XR_CORE_VECTOR3D_H

template <typename T>
struct _vector3
{
public:
    typedef T TYPE;
    typedef _vector3<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;

public:
    union
    {
        struct
        {
            T x, y, z;
        };
        std::array<T, 3> arr;
    };

    constexpr _vector3() = default;
    constexpr explicit _vector3(T _x, T _y, T _z) : x{_x}, y{_y}, z{_z} {}

    constexpr _vector3(const Self&) = default;
    constexpr _vector3(Self&&) = default;

    constexpr Self& operator=(const Self&) = default;
    constexpr Self& operator=(Self&&) = default;

    [[nodiscard]] constexpr bool operator==(const Self& that) const { return !!similar(that); }

    // access operators
    [[nodiscard]] constexpr T& operator[](gsl::index i) { return arr[gsl::narrow_cast<size_t>(i)]; }
    [[nodiscard]] constexpr const T& operator[](gsl::index i) const { return arr[gsl::narrow_cast<size_t>(i)]; }

    constexpr SelfRef set(T _x, T _y, T _z)
    {
        x = _x;
        y = _y;
        z = _z;

        return *this;
    }

    constexpr SelfRef set(const Self& v)
    {
        arr = v.arr;
        return *this;
    }

    constexpr SelfRef set(const T* p)
    {
        x = p[0];
        y = p[1];
        z = p[2];

        return *this;
    }

    constexpr SelfRef add(const Self& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    constexpr SelfRef add(T s)
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }

    constexpr SelfRef add(const Self& a, const Self& v)
    {
        x = a.x + v.x;
        y = a.y + v.y;
        z = a.z + v.z;
        return *this;
    }

    constexpr SelfRef add(const Self& a, T s)
    {
        x = a.x + s;
        y = a.y + s;
        z = a.z + s;
        return *this;
    }

    constexpr SelfRef sub(const Self& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    constexpr SelfRef sub(T s)
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }

    constexpr SelfRef sub(const Self& a, const Self& v)
    {
        x = a.x - v.x;
        y = a.y - v.y;
        z = a.z - v.z;
        return *this;
    }

    constexpr SelfRef sub(const Self& a, T s)
    {
        x = a.x - s;
        y = a.y - s;
        z = a.z - s;
        return *this;
    }

    constexpr SelfRef mul(const Self& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    constexpr SelfRef mul(T s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    constexpr SelfRef mul(const Self& a, const Self& v)
    {
        x = a.x * v.x;
        y = a.y * v.y;
        z = a.z * v.z;
        return *this;
    }

    constexpr SelfRef mul(const Self& a, T s)
    {
        x = a.x * s;
        y = a.y * s;
        z = a.z * s;
        return *this;
    }

    constexpr SelfRef div(const Self& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    constexpr SelfRef div(T s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    constexpr SelfRef div(const Self& a, const Self& v)
    {
        x = a.x / v.x;
        y = a.y / v.y;
        z = a.z / v.z;
        return *this;
    }

    constexpr SelfRef div(const Self& a, T s)
    {
        x = a.x / s;
        y = a.y / s;
        z = a.z / s;
        return *this;
    }

    constexpr SelfRef invert()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    constexpr SelfRef invert(const Self& a)
    {
        x = -a.x;
        y = -a.y;
        z = -a.z;
        return *this;
    }

    constexpr SelfRef min(const Self& v1, const Self& v2)
    {
        x = _min(v1.x, v2.x);
        y = _min(v1.y, v2.y);
        z = _min(v1.z, v2.z);
        return *this;
    }

    constexpr SelfRef min(const Self& v)
    {
        x = _min(x, v.x);
        y = _min(y, v.y);
        z = _min(z, v.z);
        return *this;
    }

    constexpr SelfRef max(const Self& v1, const Self& v2)
    {
        x = _max(v1.x, v2.x);
        y = _max(v1.y, v2.y);
        z = _max(v1.z, v2.z);
        return *this;
    }

    constexpr SelfRef max(const Self& v)
    {
        x = _max(x, v.x);
        y = _max(y, v.y);
        z = _max(z, v.z);
        return *this;
    }

    constexpr SelfRef abs(const Self& v)
    {
        x = _abs(v.x);
        y = _abs(v.y);
        z = _abs(v.z);
        return *this;
    }

    [[nodiscard]] constexpr BOOL similar(const Self& v, T E = EPS_L) const
    {
        if constexpr (std::is_floating_point_v<T>)
            return fsimilar(x, v.x, E) && fsimilar(y, v.y, E) && fsimilar(z, v.z, E);
        else
            return x == v.x && y == v.y && z == v.z;
    }

    constexpr SelfRef set_length(T l)
    {
        mul(l / magnitude());
        return *this;
    }

    // Align vector3 by axis (!y)
    constexpr SelfRef align()
    {
        y = 0;
        if (_abs(z) >= _abs(x))
        {
            z /= _abs(z ? z : 1);
            x = 0;
        }
        else
        {
            x /= _abs(x);
            z = 0;
        }
        return *this;
    }

    // Squeeze
    constexpr SelfRef squeeze(T Epsilon)
    {
        if (_abs(x) < Epsilon)
            x = 0;
        if (_abs(y) < Epsilon)
            y = 0;
        if (_abs(z) < Epsilon)
            z = 0;
        return *this;
    }

    // Clamp vector3
    constexpr SelfRef clamp(const Self& min, const Self& max) // https://github.com/OpenXRay/xray-16/commit/e1cca82fbd1f94e93f0651e676cdcbbd70a04655
    {
        ::clamp(x, min.x, max.x);
        ::clamp(y, min.y, max.y);
        ::clamp(z, min.z, max.z);
        return *this;
    }

    constexpr SelfRef clamp(const Self& _v)
    {
        Self v;
        v.x = _abs(_v.x);
        v.y = _abs(_v.y);
        v.z = _abs(_v.z);
        ::clamp(x, -v.x, v.x);
        ::clamp(y, -v.y, v.y);
        ::clamp(z, -v.z, v.z);
        return *this;
    }

    // Interpolate vectors (inertion)
    constexpr SelfRef inertion(const Self& p, T v)
    {
        T inv = 1.f - v;
        x = v * x + inv * p.x;
        y = v * y + inv * p.y;
        z = v * z + inv * p.z;
        return *this;
    }

    constexpr SelfRef average(const Self& p)
    {
        x = (x + p.x) * 0.5f;
        y = (y + p.y) * 0.5f;
        z = (z + p.z) * 0.5f;
        return *this;
    }

    constexpr SelfRef average(const Self& p1, const Self& p2)
    {
        x = (p1.x + p2.x) * 0.5f;
        y = (p1.y + p2.y) * 0.5f;
        z = (p1.z + p2.z) * 0.5f;
        return *this;
    }

    constexpr SelfRef lerp(const Self& p1, const Self& p2, T t)
    {
        T invt = 1.f - t;
        x = p1.x * invt + p2.x * t;
        y = p1.y * invt + p2.y * t;
        z = p1.z * invt + p2.z * t;
        return *this;
    }

    // Direct vector3 from point P by dir D with length M
    constexpr SelfRef mad(const Self& d, T m)
    {
        x += d.x * m;
        y += d.y * m;
        z += d.z * m;
        return *this;
    }

    constexpr SelfRef mad(const Self& p, const Self& d, T m)
    {
        x = p.x + d.x * m;
        y = p.y + d.y * m;
        z = p.z + d.z * m;
        return *this;
    }

    constexpr SelfRef mad(const Self& d, const Self& s)
    {
        x += d.x * s.x;
        y += d.y * s.y;
        z += d.z * s.z;
        return *this;
    }

    constexpr SelfRef mad(const Self& p, const Self& d, const Self& s)
    {
        x = p.x + d.x * s.x;
        y = p.y + d.y * s.y;
        z = p.z + d.z * s.z;
        return *this;
    }

    // SQ magnitude
    [[nodiscard]] constexpr T square_magnitude() const { return x * x + y * y + z * z; }
    // magnitude
    [[nodiscard]] constexpr T magnitude() const { return _sqrt(square_magnitude()); }

    // Normalize
    [[nodiscard]] constexpr T normalize_magn()
    {
        VERIFY(square_magnitude() > std::numeric_limits<T>::min());
        T len = magnitude();
        T inv_len = T(1) / len;
        x *= inv_len;
        y *= inv_len;
        z *= inv_len;
        return len;
    }

    constexpr SelfRef normalize()
    {
        VERIFY(square_magnitude() > std::numeric_limits<T>::min());

        T mag = _sqrt(T{1} / (x * x + y * y + z * z));
        x *= mag;
        y *= mag;
        z *= mag;

        return *this;
    }

    // Safe-Normalize
    constexpr SelfRef normalize_safe()
    {
        T magnitude = x * x + y * y + z * z;
        if (magnitude > std::numeric_limits<T>::min())
        {
            magnitude = _sqrt(1 / magnitude);
            x *= magnitude;
            y *= magnitude;
            z *= magnitude;
        }
        return *this;
    }

    // Normalize
    constexpr SelfRef normalize(const Self& v)
    {
        VERIFY((v.x * v.x + v.y * v.y + v.z * v.z) > flt_zero);
        T mag = _sqrt(1 / (v.x * v.x + v.y * v.y + v.z * v.z));
        x = v.x * mag;
        y = v.y * mag;
        z = v.z * mag;
        return *this;
    }

    // Safe-Normalize
    constexpr SelfRef normalize_safe(const Self& v)
    {
        T magnitude = v.x * v.x + v.y * v.y + v.z * v.z;
        if (magnitude > std::numeric_limits<T>::min())
        {
            magnitude = _sqrt(1 / magnitude);
            x = v.x * magnitude;
            y = v.y * magnitude;
            z = v.z * magnitude;
        }
        return *this;
    }

    constexpr SelfRef random_dir(CRandom& R = ::Random)
    {
        // z	= R.randF(-1,1);
        z = _cos(R.randF(PI));
        T a = R.randF(PI_MUL_2);
        T r = _sqrt(1 - z * z);
        T sa = _sin(a);
        T ca = _cos(a);
        x = r * ca;
        y = r * sa;
        return *this;
    }

    constexpr SelfRef random_dir(const Self& ConeAxis, float ConeAngle, CRandom& R = ::Random)
    {
        Self rnd;
        rnd.random_dir(R);
        mad(ConeAxis, rnd, R.randF(tanf(ConeAngle)));
        normalize();
        return *this;
    }

    constexpr SelfRef random_point(const Self& BoxSize, CRandom& R = ::Random)
    {
        x = R.randFs(BoxSize.x);
        y = R.randFs(BoxSize.y);
        z = R.randFs(BoxSize.z);
        return *this;
    }

    constexpr SelfRef random_point(T r, CRandom& R = ::Random)
    {
        random_dir(R);
        mul(R.randF(r));
        return *this;
    }

    // DotProduct
    constexpr T dotproduct(const Self& v) const // v1*v2
    {
        return x * v.x + y * v.y + z * v.z;
    }

    // CrossProduct
    constexpr SelfRef crossproduct(const Self& v1, const Self& v2) // (v1,v2) -> this
    {
        x = v1.y * v2.z - v1.z * v2.y;
        y = v1.z * v2.x - v1.x * v2.z;
        z = v1.x * v2.y - v1.y * v2.x;
        return *this;
    }

    // Distance calculation
    [[nodiscard]] constexpr T distance_to_xz(const Self& v) const { return _sqrt((x - v.x) * (x - v.x) + (z - v.z) * (z - v.z)); }
    [[nodiscard]] constexpr T distance_to_xz_sqr(const Self& v) const { return (x - v.x) * (x - v.x) + (z - v.z) * (z - v.z); }

    // Distance calculation
    [[nodiscard]] constexpr T distance_to_sqr(const Self& v) const { return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z); }
    // Distance calculation
    [[nodiscard]] constexpr T distance_to(const Self& v) const { return _sqrt(distance_to_sqr(v)); }

    // Barycentric coords
    constexpr SelfRef from_bary(const Self& V1, const Self& V2, const Self& V3, T u, T v, T w)
    {
        x = V1.x * u + V2.x * v + V3.x * w;
        y = V1.y * u + V2.y * v + V3.y * w;
        z = V1.z * u + V2.z * v + V3.z * w;
        return *this;
    }

    constexpr SelfRef from_bary(const Self& V1, const Self& V2, const Self& V3, const Self& B)
    {
        from_bary(V1, V2, V3, B.x, B.y, B.z);
        return *this;
    }

    constexpr SelfRef from_bary4(const Self& V1, const Self& V2, const Self& V3, const Self& V4, T u, T v, T w, T t)
    {
        x = V1.x * u + V2.x * v + V3.x * w + V4.x * t;
        y = V1.y * u + V2.y * v + V3.y * w + V4.y * t;
        z = V1.z * u + V2.z * v + V3.z * w + V4.z * t;
        return *this;
    }

    constexpr SelfRef mknormal_non_normalized(const Self& p0, const Self& p1, const Self& p2)
    {
        _vector3<T> v01, v12;
        v01.sub(p1, p0);
        v12.sub(p2, p1);
        crossproduct(v01, v12);
        return *this;
    }

    constexpr SelfRef mknormal(const Self& p0, const Self& p1, const Self& p2)
    {
        mknormal_non_normalized(p0, p1, p2);
        normalize_safe();
        return *this;
    }

    constexpr SelfRef setHP(T h, T p)
    {
        T _ch = _cos(h), _cp = _cos(p), _sh = _sin(h), _sp = _sin(p);
        x = -_cp * _sh;
        y = _sp;
        z = _cp * _ch;
        return *this;
    }

    constexpr void getHP(T& h, T& p) const
    {
        float hyp;

        if (fis_zero(x) && fis_zero(z))
        {
            h = 0.0f;
            if (!fis_zero(float(y)))
                p = (y > 0.0f) ? PI_DIV_2 : -PI_DIV_2;
            else
                p = 0.0f;
        }
        else
        {
            if (fis_zero(z))
                h = (x > 0.0f) ? -PI_DIV_2 : PI_DIV_2;
            else if (z < 0.0f)
                h = -(atanf(x / z) - PI);
            else
                h = -atanf(x / z);
            hyp = _sqrt(x * x + z * z);
            if (fis_zero(float(hyp)))
                p = (y > 0.0f) ? PI_DIV_2 : -PI_DIV_2;
            else
                p = atanf(y / hyp);
        }
    }

    [[nodiscard]] constexpr float getH() const
    {
        if (fis_zero(x) && fis_zero(z))
        {
            return 0.0f;
        }
        else
        {
            if (fis_zero(z))
                return (x > 0.0f) ? -PI_DIV_2 : PI_DIV_2;
            else if (z < 0.0f)
                return -(atanf(x / z) - PI);
            else
                return -atanf(x / z);
        }
    }

    [[nodiscard]] constexpr float getP() const
    {
        if (fis_zero(x) && fis_zero(z))
        {
            if (!fis_zero(float(y)))
                return (y > 0.0f) ? PI_DIV_2 : -PI_DIV_2;
            else
                return 0.0f;
        }
        else
        {
            float hyp = _sqrt(x * x + z * z);
            if (fis_zero(float(hyp)))
                return (y > 0.0f) ? PI_DIV_2 : -PI_DIV_2;
            else
                return atanf(y / hyp);
        }
    }

    constexpr SelfRef reflect(const Self& dir, const Self& norm) { return mad(dir, norm, -2 * dir.dotproduct(norm)); }

    constexpr SelfRef slide(const Self& dir, const Self& norm)
    {
        // non normalized
        return mad(dir, norm, -dir.dotproduct(norm));
    }

    constexpr static void generate_orthonormal_basis(const _vector3<T>& dir, _vector3<T>& up, _vector3<T>& right)
    {
        T fInvLength;

        if (_abs(dir.x) >= _abs(dir.y))
        {
            // W.x or W.z is the largest magnitude component, swap them
            fInvLength = 1.f / _sqrt(dir.x * dir.x + dir.z * dir.z);
            up.x = -dir.z * fInvLength;
            up.y = 0.0f;
            up.z = +dir.x * fInvLength;
        }
        else
        {
            // W.y or W.z is the largest magnitude component, swap them
            fInvLength = 1.f / _sqrt(dir.y * dir.y + dir.z * dir.z);
            up.x = 0.0f;
            up.y = +dir.z * fInvLength;
            up.z = -dir.y * fInvLength;
        }

        right.crossproduct(up, dir); //. <->
    }

    constexpr static void generate_orthonormal_basis_normalized(_vector3<T>& dir, _vector3<T>& up, _vector3<T>& right)
    {
        T fInvLength;
        dir.normalize();
        if (fsimilar(dir.y, 1.f, EPS))
        {
            up.set(0.f, 0.f, 1.f);
            fInvLength = 1.f / _sqrt(dir.x * dir.x + dir.y * dir.y);
            // cross (up,dir) and normalize (right)
            right.x = -dir.y * fInvLength;
            right.y = dir.x * fInvLength;
            right.z = 0.f;
            // cross (dir,right)
            up.x = -dir.z * right.y;
            up.y = dir.z * right.x;
            up.z = dir.x * right.y - dir.y * right.x;
        }
        else
        {
            up.set(0.f, 1.f, 0.f);
            fInvLength = 1.f / _sqrt(dir.x * dir.x + dir.z * dir.z);
            // cross (up,dir) and normalize (right)
            right.x = dir.z * fInvLength;
            right.y = 0.f;
            right.z = -dir.x * fInvLength;
            // cross (dir,right)
            up.x = dir.y * right.z;
            up.y = dir.z * right.x - dir.x * right.z;
            up.z = -dir.y * right.x;
        }
    }
};

using Fvector = _vector3<f32>;
using Fvector3 = _vector3<f32>;
static_assert(sizeof(Fvector) == 12);

using Ivector = _vector3<s32>;
using Ivector3 = _vector3<s32>;
static_assert(sizeof(Ivector) == 12);

template <typename T>
[[nodiscard]] constexpr bool _valid(const _vector3<T>& v)
{
    return _valid(v.x) && _valid(v.y) && _valid(v.z);
}

//////////////////////////////////////////////////////////////////////////

[[nodiscard]] constexpr bool exact_normalize(std::array<f32, 3>& a)
{
    constexpr auto rsqrt = [](f64 v) -> f64 { return 1.0 / _sqrt(v); };
    f64 l, a0{a[0]}, a1{a[1]}, a2{a[2]};
    const f64 sqr_magnitude{a0 * a0 + a1 * a1 + a2 * a2};
    constexpr f64 epsilon{1.192092896e-05};

    if (sqr_magnitude > epsilon)
    {
        l = rsqrt(sqr_magnitude);
        a[0] = gsl::narrow_cast<f32>(a0 * l);
        a[1] = gsl::narrow_cast<f32>(a1 * l);
        a[2] = gsl::narrow_cast<f32>(a2 * l);

        return true;
    }

    f64 aa0{_abs(a0)}, aa1{_abs(a1)}, aa2{_abs(a2)};
    if (aa1 > aa0)
    {
        if (aa2 > aa1)
        {
            goto aa2_largest;
        }
        else
        {
            // aa1 is largest
            a0 /= aa1;
            a2 /= aa1;

            l = rsqrt(a0 * a0 + a2 * a2 + 1);
            a[0] = gsl::narrow_cast<f32>(a0 * l);
            a[1] = gsl::narrow_cast<f32>(std::copysign(l, a1));
            a[2] = gsl::narrow_cast<f32>(a2 * l);
        }
    }
    else
    {
        if (aa2 > aa0)
        {
        aa2_largest:
            // aa2 is largest
            a0 /= aa2;
            a1 /= aa2;

            l = rsqrt(a0 * a0 + a1 * a1 + 1);
            a[0] = gsl::narrow_cast<f32>(a0 * l);
            a[1] = gsl::narrow_cast<f32>(a1 * l);
            a[2] = gsl::narrow_cast<f32>(std::copysign(l, a2));
        }
        else
        {
            // aa0 is largest
            if (aa0 <= 0)
            {
                // dDEBUGMSG ("vector has zero size"); ... this messace is annoying
                a[0] = 0.0f; // if all a's are zero, this is where we'll end up.
                a[1] = 1.0f; // return a default unit length vector.
                a[2] = 0.0f;

                return false;
            }

            a1 /= aa0;
            a2 /= aa0;

            l = rsqrt(a1 * a1 + a2 * a2 + 1);
            a[0] = gsl::narrow_cast<f32>(std::copysign(l, a0));
            a[1] = gsl::narrow_cast<f32>(a1 * l);
            a[2] = gsl::narrow_cast<f32>(a2 * l);
        }
    }

    return true;
}

[[nodiscard]] constexpr bool exact_normalize(Fvector3& a) { return exact_normalize(a.arr); }

#endif /* __XR_CORE_VECTOR3D_H */
