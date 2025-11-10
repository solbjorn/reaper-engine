#ifndef __XR_CORE_VECTOR4_H
#define __XR_CORE_VECTOR4_H

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wdefaulted-function-deleted");
XR_DIAG_IGNORE("-Wmissing-variable-declarations");

#include <DirectXMath.h>

XR_DIAG_POP();

template <typename T>
struct XR_TRIVIAL alignas(16) _ivector4
{
    typedef T TYPE;
    typedef _ivector4<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;

private:
    union
    {
        DirectX::XMVECTORI32 cv;
        DirectX::XMVECTOR mv;
        std::array<T, 4> arr;
    };

public:
    constexpr _ivector4() = default;
    constexpr explicit _ivector4(T x, T y, T z, T w) noexcept { set(x, y, z, w); }

    constexpr _ivector4(const Self& that) noexcept { set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _ivector4(Self&&) noexcept = default;
#else
    constexpr _ivector4(Self&& that) noexcept { set(that); }
#endif

    constexpr Self& operator=(const Self& that) noexcept { return set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr Self& operator=(Self&&) noexcept = default;
#else
    constexpr Self& operator=(Self&& that) noexcept { return set(that); }
#endif

    [[nodiscard]] constexpr T getx() const { return cv.i[0]; }
    [[nodiscard]] constexpr T gety() const { return cv.i[1]; }
    [[nodiscard]] constexpr T getz() const { return cv.i[2]; }
    [[nodiscard]] constexpr T getw() const { return cv.i[3]; }
    constexpr void setx(T x) { cv.i[0] = x; }
    constexpr void sety(T y) { cv.i[1] = y; }
    constexpr void setz(T z) { cv.i[2] = z; }
    constexpr void setw(T w) { cv.i[3] = w; }

    __declspec(property(get = getx, put = setx)) T x;
    __declspec(property(get = gety, put = sety)) T y;
    __declspec(property(get = getz, put = setz)) T z;
    __declspec(property(get = getw, put = setw)) T w;

    [[nodiscard]] constexpr T& operator[](gsl::index i) { return arr[gsl::narrow_cast<size_t>(i)]; }
    [[nodiscard]] constexpr const T& operator[](gsl::index i) const { return arr[gsl::narrow_cast<size_t>(i)]; }

    constexpr SelfRef set(T _x, T _y, T _z, T _w = 1)
    {
        cv = DirectX::XMVECTORI32{.i = {_x, _y, _z, _w}};
        return *this;
    }

    constexpr SelfRef set(const Self& v)
    {
        xr_memcpy16(&mv, &v.mv);
        return *this;
    }

    constexpr SelfRef add(const Self& v)
    {
        cv.i[0] += v.x;
        cv.i[1] += v.y;
        cv.i[2] += v.z;
        cv.i[3] += v.w;

        return *this;
    }

    constexpr SelfRef add(T s)
    {
        cv.i[0] += s;
        cv.i[1] += s;
        cv.i[2] += s;
        cv.i[3] += s;

        return *this;
    }

    constexpr SelfRef add(const Self& a, const Self& v)
    {
        cv.i[0] = a.x + v.x;
        cv.i[1] = a.y + v.y;
        cv.i[2] = a.z + v.z;
        cv.i[3] = a.w + v.w;

        return *this;
    }

    constexpr SelfRef add(const Self& a, T s)
    {
        cv.i[0] = a.x + s;
        cv.i[1] = a.y + s;
        cv.i[2] = a.z + s;
        cv.i[3] = a.w + s;

        return *this;
    }

    constexpr SelfRef sub(T _x, T _y, T _z, T _w = 1)
    {
        cv.i[0] -= _x;
        cv.i[1] -= _y;
        cv.i[2] -= _z;
        cv.i[3] -= _w;

        return *this;
    }

    constexpr SelfRef sub(const Self& v)
    {
        cv.i[0] -= v.x;
        cv.i[1] -= v.y;
        cv.i[2] -= v.z;
        cv.i[3] -= v.w;

        return *this;
    }

    constexpr SelfRef sub(T s)
    {
        cv.i[0] -= s;
        cv.i[1] -= s;
        cv.i[2] -= s;
        cv.i[3] -= s;

        return *this;
    }

    constexpr SelfRef sub(const Self& a, const Self& v)
    {
        cv.i[0] = a.x - v.x;
        cv.i[1] = a.y - v.y;
        cv.i[2] = a.z - v.z;
        cv.i[3] = a.w - v.w;

        return *this;
    }

    constexpr SelfRef sub(const Self& a, T s)
    {
        cv.i[0] = a.x - s;
        cv.i[1] = a.y - s;
        cv.i[2] = a.z - s;
        cv.i[3] = a.w - s;

        return *this;
    }

    constexpr SelfRef mul(T _x, T _y, T _z, T _w = 1)
    {
        cv.i[0] *= _x;
        cv.i[1] *= _y;
        cv.i[2] *= _z;
        cv.i[3] *= _w;

        return *this;
    }

    constexpr SelfRef mul(const Self& v)
    {
        cv.i[0] *= v.x;
        cv.i[1] *= v.y;
        cv.i[2] *= v.z;
        cv.i[3] *= v.w;

        return *this;
    }

    constexpr SelfRef mul(T s)
    {
        cv.i[0] *= s;
        cv.i[1] *= s;
        cv.i[2] *= s;
        cv.i[3] *= s;

        return *this;
    }

    constexpr SelfRef mul(const Self& a, const Self& v)
    {
        cv.i[0] = a.x * v.x;
        cv.i[1] = a.y * v.y;
        cv.i[2] = a.z * v.z;
        cv.i[3] = a.w * v.w;

        return *this;
    }

    constexpr SelfRef mul(const Self& a, T s)
    {
        cv.i[0] = a.x * s;
        cv.i[1] = a.y * s;
        cv.i[2] = a.z * s;
        cv.i[3] = a.w * s;

        return *this;
    }

    constexpr SelfRef div(const Self& v)
    {
        cv.i[0] /= v.x;
        cv.i[1] /= v.y;
        cv.i[2] /= v.z;
        cv.i[3] /= v.w;

        return *this;
    }

    constexpr SelfRef div(T s)
    {
        cv.i[0] /= s;
        cv.i[1] /= s;
        cv.i[2] /= s;
        cv.i[3] /= s;

        return *this;
    }

    constexpr SelfRef div(const Self& a, const Self& v)
    {
        cv.i[0] = a.x / v.x;
        cv.i[1] = a.y / v.y;
        cv.i[2] = a.z / v.z;
        cv.i[3] = a.w / v.w;

        return *this;
    }

    constexpr SelfRef div(const Self& a, T s)
    {
        cv.i[0] = a.x / s;
        cv.i[1] = a.y / s;
        cv.i[2] = a.z / s;
        cv.i[3] = a.w / s;

        return *this;
    }
};

template <typename T>
struct XR_TRIVIAL alignas(16) _fvector4
{
    typedef T TYPE;
    typedef _vector3<T> Tvector;
    typedef _fvector4<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;

private:
    union
    {
        DirectX::XMVECTORF32 cv;
        DirectX::XMVECTOR mv;
        std::array<T, 4> arr;
    };

public:
    constexpr _fvector4() noexcept = default;
    constexpr explicit _fvector4(T x, T y, T z, T w) noexcept { set(x, y, z, w); }

    constexpr _fvector4(const Self& that) noexcept { set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _fvector4(Self&&) noexcept = default;
#else
    constexpr _fvector4(Self&& that) noexcept { set(that); }
#endif

    constexpr Self& operator=(const Self& that) noexcept { return set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr Self& operator=(Self&&) noexcept = default;
#else
    constexpr Self& operator=(Self&& that) noexcept { return set(that); }
#endif

    [[nodiscard]] constexpr T getx() const
    {
        if (std::is_constant_evaluated())
            return cv.f[0];
        else
            return DirectX::XMVectorGetX(mv);
    }

    [[nodiscard]] constexpr T gety() const
    {
        if (std::is_constant_evaluated())
            return cv.f[1];
        else
            return DirectX::XMVectorGetY(mv);
    }

    [[nodiscard]] constexpr T getz() const
    {
        if (std::is_constant_evaluated())
            return cv.f[2];
        else
            return DirectX::XMVectorGetZ(mv);
    }

    [[nodiscard]] constexpr T getw() const
    {
        if (std::is_constant_evaluated())
            return cv.f[3];
        else
            return DirectX::XMVectorGetW(mv);
    }

    constexpr void setx(T x)
    {
        if (std::is_constant_evaluated())
            cv.f[0] = x;
        else
            mv = DirectX::XMVectorSetX(mv, x);
    }

    constexpr void sety(T y)
    {
        if (std::is_constant_evaluated())
            cv.f[1] = y;
        else
            mv = DirectX::XMVectorSetY(mv, y);
    }

    constexpr void setz(T z)
    {
        if (std::is_constant_evaluated())
            cv.f[2] = z;
        else
            mv = DirectX::XMVectorSetZ(mv, z);
    }

    constexpr void setw(T w)
    {
        if (std::is_constant_evaluated())
            cv.f[3] = w;
        else
            mv = DirectX::XMVectorSetW(mv, w);
    }

    __declspec(property(get = getx, put = setx)) T x;
    __declspec(property(get = gety, put = sety)) T y;
    __declspec(property(get = getz, put = setz)) T z;
    __declspec(property(get = getw, put = setw)) T w;

    [[nodiscard]] constexpr T& operator[](gsl::index i) { return arr[gsl::narrow_cast<size_t>(i)]; }
    [[nodiscard]] constexpr const T& operator[](gsl::index i) const { return arr[gsl::narrow_cast<size_t>(i)]; }

    constexpr SelfRef set(T _x, T _y, T _z, T _w = 1.0f)
    {
        if (std::is_constant_evaluated())
            cv = DirectX::XMVECTORF32{.f = {_x, _y, _z, _w}};
        else
            mv = DirectX::XMVectorSet(_x, _y, _z, _w);

        return *this;
    }

    constexpr SelfRef set(const Tvector& vec, T _w = 1.0f)
    {
        if (std::is_constant_evaluated())
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = _w;
        }
        else
        {
            mv = DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&vec));
            w = _w;
        }

        return *this;
    }

    constexpr SelfRef set(const Self& v)
    {
        xr_memcpy16(&mv, &v.mv);
        return *this;
    }

    constexpr SelfRef add(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] += v.x;
            cv.f[1] += v.y;
            cv.f[2] += v.z;
            cv.f[3] += v.w;
        }
        else
        {
            mv = DirectX::XMVectorAdd(mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef add(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] += s;
            cv.f[1] += s;
            cv.f[2] += s;
            cv.f[3] += s;
        }
        else
        {
            mv = DirectX::XMVectorAdd(mv, DirectX::XMVectorReplicate(s));
        }

        return *this;
    }

    constexpr SelfRef add(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x + v.x;
            cv.f[1] = a.y + v.y;
            cv.f[2] = a.z + v.z;
            cv.f[3] = a.w + v.w;
        }
        else
        {
            mv = DirectX::XMVectorAdd(a.mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef add(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x + s;
            cv.f[1] = a.y + s;
            cv.f[2] = a.z + s;
            cv.f[3] = a.w + s;
        }
        else
        {
            mv = DirectX::XMVectorAdd(a.mv, DirectX::XMVectorReplicate(s));
        }

        return *this;
    }

    constexpr SelfRef sub(T _x, T _y, T _z, T _w = 1.0f)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] -= _x;
            cv.f[1] -= _y;
            cv.f[2] -= _z;
            cv.f[3] -= _w;
        }
        else
        {
            mv = DirectX::XMVectorSubtract(mv, DirectX::XMVectorSet(_x, _y, _z, _w));
        }

        return *this;
    }

    constexpr SelfRef sub(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] -= v.x;
            cv.f[1] -= v.y;
            cv.f[2] -= v.z;
            cv.f[3] -= v.w;
        }
        else
        {
            mv = DirectX::XMVectorSubtract(mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef sub(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] -= s;
            cv.f[1] -= s;
            cv.f[2] -= s;
            cv.f[3] -= s;
        }
        else
        {
            mv = DirectX::XMVectorSubtract(mv, DirectX::XMVectorReplicate(s));
        }

        return *this;
    }

    constexpr SelfRef sub(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x - v.x;
            cv.f[1] = a.y - v.y;
            cv.f[2] = a.z - v.z;
            cv.f[3] = a.w - v.w;
        }
        else
        {
            mv = DirectX::XMVectorSubtract(a.mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef sub(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x - s;
            cv.f[1] = a.y - s;
            cv.f[2] = a.z - s;
            cv.f[3] = a.w - s;
        }
        else
        {
            mv = DirectX::XMVectorSubtract(a.mv, DirectX::XMVectorReplicate(s));
        }

        return *this;
    }

    constexpr SelfRef mul(T _x, T _y, T _z, T _w = 1.0f)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] *= _x;
            cv.f[1] *= _y;
            cv.f[2] *= _z;
            cv.f[3] *= _w;
        }
        else
        {
            mv = DirectX::XMVectorMultiply(mv, DirectX::XMVectorSet(_x, _y, _z, _w));
        }

        return *this;
    }

    constexpr SelfRef mul(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] *= v.x;
            cv.f[1] *= v.y;
            cv.f[2] *= v.z;
            cv.f[3] *= v.w;
        }
        else
        {
            mv = DirectX::XMVectorMultiply(mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef mul(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] *= s;
            cv.f[1] *= s;
            cv.f[2] *= s;
            cv.f[3] *= s;
        }
        else
        {
            mv = DirectX::XMVectorScale(mv, s);
        }

        return *this;
    }

    constexpr SelfRef mul(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x * v.x;
            cv.f[1] = a.y * v.y;
            cv.f[2] = a.z * v.z;
            cv.f[3] = a.w * v.w;
        }
        else
        {
            mv = DirectX::XMVectorMultiply(a.mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef mul(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x * s;
            cv.f[1] = a.y * s;
            cv.f[2] = a.z * s;
            cv.f[3] = a.w * s;
        }
        else
        {
            mv = DirectX::XMVectorScale(a.mv, s);
        }

        return *this;
    }

    constexpr SelfRef div(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] /= v.x;
            cv.f[1] /= v.y;
            cv.f[2] /= v.z;
            cv.f[3] /= v.w;
        }
        else
        {
            mv = DirectX::XMVectorDivide(mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef div(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] /= s;
            cv.f[1] /= s;
            cv.f[2] /= s;
            cv.f[3] /= s;
        }
        else
        {
            mv = DirectX::XMVectorDivide(mv, DirectX::XMVectorReplicate(s));
        }

        return *this;
    }

    constexpr SelfRef div(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x / v.x;
            cv.f[1] = a.y / v.y;
            cv.f[2] = a.z / v.z;
            cv.f[3] = a.w / v.w;
        }
        else
        {
            mv = DirectX::XMVectorDivide(a.mv, v.mv);
        }

        return *this;
    }

    constexpr SelfRef div(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x / s;
            cv.f[1] = a.y / s;
            cv.f[2] = a.z / s;
            cv.f[3] = a.w / s;
        }
        else
        {
            mv = DirectX::XMVectorDivide(a.mv, DirectX::XMVectorReplicate(s));
        }

        return *this;
    }

    [[nodiscard]] constexpr bool similar(const Self& v, T E = EPS_L) const
    {
        if (std::is_constant_evaluated())
            return fsimilar(cv.f[0], v.x, E) && fsimilar(cv.f[1], v.y, E) && fsimilar(cv.f[2], v.z, E) && fsimilar(cv.f[3], v.w, E);
        else
            return DirectX::XMVector4NearEqual(mv, v.mv, DirectX::XMVectorReplicate(E));
    }

    [[nodiscard]] constexpr T magnitude_sqr() const
    {
        if (std::is_constant_evaluated())
            return cv.f[0] * cv.f[0] + cv.f[1] * cv.f[1] + cv.f[2] * cv.f[2] + cv.f[3] * cv.f[3];
        else
            return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(mv));
    }

    [[nodiscard]] constexpr T magnitude() const
    {
        if (std::is_constant_evaluated())
            return _sqrt(magnitude_sqr());
        else
            return DirectX::XMVectorGetX(DirectX::XMVector4Length(mv));
    }

    constexpr SelfRef normalize()
    {
        if (std::is_constant_evaluated())
            mul(1 / magnitude());
        else
            mv = DirectX::XMVector4Normalize(mv);

        return *this;
    }

    constexpr SelfRef normalize_as_plane()
    {
        if (std::is_constant_evaluated())
        {
            mul(1 / _sqrt(cv.f[0] * cv.f[0] + cv.f[1] * cv.f[1] + cv.f[2] * cv.f[2]));
        }
        else
        {
            DirectX::XMVECTOR tmp = DirectX::XMVector3ReciprocalLength(mv);
            mv = DirectX::XMVectorMultiply(mv, tmp);
        }

        return *this;
    }

    constexpr SelfRef lerp(const Self& p1, const Self& p2, T t)
    {
        if (std::is_constant_evaluated())
        {
            const T invt{1.0f - t};

            cv.f[0] = p1.x * invt + p2.x * t;
            cv.f[1] = p1.y * invt + p2.y * t;
            cv.f[2] = p1.z * invt + p2.z * t;
            cv.f[3] = p1.w * invt + p2.w * t;
        }
        else
        {
            mv = DirectX::XMVectorLerp(p1.mv, p2.mv, t);
        }

        return *this;
    }
};

using Fvector4 = _fvector4<f32>;
static_assert(sizeof(Fvector4) == 16);
XR_TRIVIAL_ASSERT(Fvector4);

using Ivector4 = _ivector4<s32>;
static_assert(sizeof(Ivector4) == 16);
XR_TRIVIAL_ASSERT(Ivector4);

#endif /* __XR_CORE_VECTOR4_H */
