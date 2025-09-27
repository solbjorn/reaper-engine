#ifndef __XR_CORE_VECTOR4_H
#define __XR_CORE_VECTOR4_H

#include <DirectXMath.h>

template <class T>
struct alignas(16) _ivector4
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
    };

public:
    constexpr inline _ivector4(T x = 0, T y = 0, T z = 0, T w = 0) noexcept { set(x, y, z, w); }

    constexpr inline _ivector4(const Self& v) noexcept { set(v); }
    constexpr _ivector4(Self&& that) noexcept { *this = std::move(that); }

    constexpr inline T getx() const { return cv.i[0]; }
    constexpr inline T gety() const { return cv.i[1]; }
    constexpr inline T getz() const { return cv.i[2]; }
    constexpr inline T getw() const { return cv.i[3]; }
    constexpr inline void setx(T x) { cv.i[0] = x; }
    constexpr inline void sety(T y) { cv.i[1] = y; }
    constexpr inline void setz(T z) { cv.i[2] = z; }
    constexpr inline void setw(T w) { cv.i[3] = w; }

    __declspec(property(get = getx, put = setx)) T x;
    __declspec(property(get = gety, put = sety)) T y;
    __declspec(property(get = getz, put = setz)) T z;
    __declspec(property(get = getw, put = setw)) T w;

    constexpr inline SelfRef operator=(const Self& v) noexcept { return set(v); }

    constexpr Self& operator=(Self&& that) noexcept
    {
        if (std::is_constant_evaluated())
            this->cv = std::move(that.cv);
        else
            this->mv = std::move(that.mv);

        return *this;
    }

    constexpr inline T& operator[](int i) { return *((T*)this + i); }
    constexpr inline T& operator[](int i) const { return *((T*)this + i); }

    constexpr inline SelfRef set(T _x, T _y, T _z, T _w = 1)
    {
        cv = DirectX::XMVECTORI32{_x, _y, _z, _w};
        return *this;
    }
    constexpr inline SelfRef set(const Self& v)
    {
        xr_memcpy16(&mv, &v.mv);
        return *this;
    }

    constexpr inline SelfRef add(const Self& v)
    {
        cv.i[0] += v.x;
        cv.i[1] += v.y;
        cv.i[2] += v.z;
        cv.i[3] += v.w;

        return *this;
    }
    constexpr inline SelfRef add(T s)
    {
        cv.i[0] += s;
        cv.i[1] += s;
        cv.i[2] += s;
        cv.i[3] += s;

        return *this;
    }
    constexpr inline SelfRef add(const Self& a, const Self& v)
    {
        cv.i[0] = a.x + v.x;
        cv.i[1] = a.y + v.y;
        cv.i[2] = a.z + v.z;
        cv.i[3] = a.w + v.w;

        return *this;
    }
    constexpr inline SelfRef add(const Self& a, T s)
    {
        cv.i[0] = a.x + s;
        cv.i[1] = a.y + s;
        cv.i[2] = a.z + s;
        cv.i[3] = a.w + s;

        return *this;
    }

    constexpr inline SelfRef sub(T _x, T _y, T _z, T _w = 1)
    {
        cv.i[0] -= _x;
        cv.i[1] -= _y;
        cv.i[2] -= _z;
        cv.i[3] -= _w;

        return *this;
    }
    constexpr inline SelfRef sub(const Self& v)
    {
        cv.i[0] -= v.x;
        cv.i[1] -= v.y;
        cv.i[2] -= v.z;
        cv.i[3] -= v.w;

        return *this;
    }
    constexpr inline SelfRef sub(T s)
    {
        cv.i[0] -= s;
        cv.i[1] -= s;
        cv.i[2] -= s;
        cv.i[3] -= s;

        return *this;
    }
    constexpr inline SelfRef sub(const Self& a, const Self& v)
    {
        cv.i[0] = a.x - v.x;
        cv.i[1] = a.y - v.y;
        cv.i[2] = a.z - v.z;
        cv.i[3] = a.w - v.w;

        return *this;
    }
    constexpr inline SelfRef sub(const Self& a, T s)
    {
        cv.i[0] = a.x - s;
        cv.i[1] = a.y - s;
        cv.i[2] = a.z - s;
        cv.i[3] = a.w - s;

        return *this;
    }

    constexpr inline SelfRef mul(T _x, T _y, T _z, T _w = 1)
    {
        cv.i[0] *= _x;
        cv.i[1] *= _y;
        cv.i[2] *= _z;
        cv.i[3] *= _w;

        return *this;
    }
    constexpr inline SelfRef mul(const Self& v)
    {
        cv.i[0] *= v.x;
        cv.i[1] *= v.y;
        cv.i[2] *= v.z;
        cv.i[3] *= v.w;

        return *this;
    }
    constexpr inline SelfRef mul(T s)
    {
        cv.i[0] *= s;
        cv.i[1] *= s;
        cv.i[2] *= s;
        cv.i[3] *= s;

        return *this;
    }
    constexpr inline SelfRef mul(const Self& a, const Self& v)
    {
        cv.i[0] = a.x * v.x;
        cv.i[1] = a.y * v.y;
        cv.i[2] = a.z * v.z;
        cv.i[3] = a.w * v.w;

        return *this;
    }
    constexpr inline SelfRef mul(const Self& a, T s)
    {
        cv.i[0] = a.x * s;
        cv.i[1] = a.y * s;
        cv.i[2] = a.z * s;
        cv.i[3] = a.w * s;

        return *this;
    }

    constexpr inline SelfRef div(const Self& v)
    {
        cv.i[0] /= v.x;
        cv.i[1] /= v.y;
        cv.i[2] /= v.z;
        cv.i[3] /= v.w;

        return *this;
    }
    constexpr inline SelfRef div(T s)
    {
        cv.i[0] /= s;
        cv.i[1] /= s;
        cv.i[2] /= s;
        cv.i[3] /= s;

        return *this;
    }
    constexpr inline SelfRef div(const Self& a, const Self& v)
    {
        cv.i[0] = a.x / v.x;
        cv.i[1] = a.y / v.y;
        cv.i[2] = a.z / v.z;
        cv.i[3] = a.w / v.w;

        return *this;
    }
    constexpr inline SelfRef div(const Self& a, T s)
    {
        cv.i[0] = a.x / s;
        cv.i[1] = a.y / s;
        cv.i[2] = a.z / s;
        cv.i[3] = a.w / s;

        return *this;
    }
};

template <class T>
struct alignas(16) _fvector4
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
    };

public:
    constexpr inline _fvector4() noexcept = default;
    constexpr inline _fvector4(T x, T y, T z, T w) noexcept { set(x, y, z, w); }

    constexpr inline _fvector4(const Self& v) noexcept { set(v); }
    constexpr _fvector4(Self&& that) noexcept { *this = std::move(that); }

    constexpr inline T getx() const
    {
        if (std::is_constant_evaluated())
            return cv.f[0];
        else
            return DirectX::XMVectorGetX(mv);
    }
    constexpr inline T gety() const
    {
        if (std::is_constant_evaluated())
            return cv.f[1];
        else
            return DirectX::XMVectorGetY(mv);
    }
    constexpr inline T getz() const
    {
        if (std::is_constant_evaluated())
            return cv.f[2];
        else
            return DirectX::XMVectorGetZ(mv);
    }
    constexpr inline T getw() const
    {
        if (std::is_constant_evaluated())
            return cv.f[3];
        else
            return DirectX::XMVectorGetW(mv);
    }
    constexpr inline void setx(T x)
    {
        if (std::is_constant_evaluated())
            cv.f[0] = x;
        else
            mv = DirectX::XMVectorSetX(mv, x);
    }
    constexpr inline void sety(T y)
    {
        if (std::is_constant_evaluated())
            cv.f[1] = y;
        else
            mv = DirectX::XMVectorSetY(mv, y);
    }
    constexpr inline void setz(T z)
    {
        if (std::is_constant_evaluated())
            cv.f[2] = z;
        else
            mv = DirectX::XMVectorSetZ(mv, z);
    }
    constexpr inline void setw(T w)
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

    constexpr inline SelfRef operator=(const Self& v) noexcept { return set(v); }

    constexpr Self& operator=(Self&& that) noexcept
    {
        if (std::is_constant_evaluated())
            this->cv = std::move(that.cv);
        else
            this->mv = std::move(that.mv);

        return *this;
    }

    constexpr inline T& operator[](int i) { return *((T*)this + i); }
    constexpr inline T& operator[](int i) const { return *((T*)this + i); }

    constexpr inline SelfRef set(T _x, T _y, T _z, T _w = 1)
    {
        if (std::is_constant_evaluated())
            cv = DirectX::XMVECTORF32{_x, _y, _z, _w};
        else
            mv = DirectX::XMVectorSet(_x, _y, _z, _w);

        return *this;
    }
    constexpr inline SelfRef set(const Tvector& vec, T _w = 1)
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
            if (_w)
                w = _w;
        }

        return *this;
    }
    constexpr inline SelfRef set(const Self& v)
    {
        xr_memcpy16(&mv, &v.mv);
        return *this;
    }

    constexpr inline SelfRef add(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] += v.x;
            cv.f[1] += v.y;
            cv.f[2] += v.z;
            cv.f[3] += v.w;
        }
        else
            mv = DirectX::XMVectorAdd(mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef add(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] += s;
            cv.f[1] += s;
            cv.f[2] += s;
            cv.f[3] += s;
        }
        else
            mv = DirectX::XMVectorAdd(mv, DirectX::XMVectorReplicate(s));

        return *this;
    }
    constexpr inline SelfRef add(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x + v.x;
            cv.f[1] = a.y + v.y;
            cv.f[2] = a.z + v.z;
            cv.f[3] = a.w + v.w;
        }
        else
            mv = DirectX::XMVectorAdd(a.mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef add(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x + s;
            cv.f[1] = a.y + s;
            cv.f[2] = a.z + s;
            cv.f[3] = a.w + s;
        }
        else
            mv = DirectX::XMVectorAdd(a.mv, DirectX::XMVectorReplicate(s));

        return *this;
    }

    constexpr inline SelfRef sub(T _x, T _y, T _z, T _w = 1)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] -= _x;
            cv.f[1] -= _y;
            cv.f[2] -= _z;
            cv.f[3] -= _w;
        }
        else
            mv = DirectX::XMVectorSubtract(mv, DirectX::XMVectorSet(_x, _y, _z, _w));

        return *this;
    }
    constexpr inline SelfRef sub(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] -= v.x;
            cv.f[1] -= v.y;
            cv.f[2] -= v.z;
            cv.f[3] -= v.w;
        }
        else
            mv = DirectX::XMVectorSubtract(mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef sub(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] -= s;
            cv.f[1] -= s;
            cv.f[2] -= s;
            cv.f[3] -= s;
        }
        else
            mv = DirectX::XMVectorSubtract(mv, DirectX::XMVectorReplicate(s));

        return *this;
    }
    constexpr inline SelfRef sub(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x - v.x;
            cv.f[1] = a.y - v.y;
            cv.f[2] = a.z - v.z;
            cv.f[3] = a.w - v.w;
        }
        else
            mv = DirectX::XMVectorSubtract(a.mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef sub(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x - s;
            cv.f[1] = a.y - s;
            cv.f[2] = a.z - s;
            cv.f[3] = a.w - s;
        }
        else
            mv = DirectX::XMVectorSubtract(a.mv, DirectX::XMVectorReplicate(s));

        return *this;
    }

    constexpr inline SelfRef mul(T _x, T _y, T _z, T _w = 1)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] *= _x;
            cv.f[1] *= _y;
            cv.f[2] *= _z;
            cv.f[3] *= _w;
        }
        else
            mv = DirectX::XMVectorMultiply(mv, DirectX::XMVectorSet(_x, _y, _z, _w));

        return *this;
    }
    constexpr inline SelfRef mul(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] *= v.x;
            cv.f[1] *= v.y;
            cv.f[2] *= v.z;
            cv.f[3] *= v.w;
        }
        else
            mv = DirectX::XMVectorMultiply(mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef mul(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] *= s;
            cv.f[1] *= s;
            cv.f[2] *= s;
            cv.f[3] *= s;
        }
        else
            mv = DirectX::XMVectorScale(mv, s);

        return *this;
    }
    constexpr inline SelfRef mul(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x * v.x;
            cv.f[1] = a.y * v.y;
            cv.f[2] = a.z * v.z;
            cv.f[3] = a.w * v.w;
        }
        else
            mv = DirectX::XMVectorMultiply(a.mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef mul(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x * s;
            cv.f[1] = a.y * s;
            cv.f[2] = a.z * s;
            cv.f[3] = a.w * s;
        }
        else
            mv = DirectX::XMVectorScale(a.mv, s);

        return *this;
    }

    constexpr inline SelfRef div(const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] /= v.x;
            cv.f[1] /= v.y;
            cv.f[2] /= v.z;
            cv.f[3] /= v.w;
        }
        else
            mv = DirectX::XMVectorDivide(mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef div(T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] /= s;
            cv.f[1] /= s;
            cv.f[2] /= s;
            cv.f[3] /= s;
        }
        else
            mv = DirectX::XMVectorDivide(mv, DirectX::XMVectorReplicate(s));

        return *this;
    }
    constexpr inline SelfRef div(const Self& a, const Self& v)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x / v.x;
            cv.f[1] = a.y / v.y;
            cv.f[2] = a.z / v.z;
            cv.f[3] = a.w / v.w;
        }
        else
            mv = DirectX::XMVectorDivide(a.mv, v.mv);

        return *this;
    }
    constexpr inline SelfRef div(const Self& a, T s)
    {
        if (std::is_constant_evaluated())
        {
            cv.f[0] = a.x / s;
            cv.f[1] = a.y / s;
            cv.f[2] = a.z / s;
            cv.f[3] = a.w / s;
        }
        else
            mv = DirectX::XMVectorDivide(a.mv, DirectX::XMVectorReplicate(s));

        return *this;
    }

    constexpr inline BOOL similar(const Self& v, T E = EPS_L)
    {
        if (std::is_constant_evaluated())
            return fsimilar(cv.f[0], v.x, E) && fsimilar(cv.f[1], v.y, E) && fsimilar(cv.f[2], v.z, E) && fsimilar(cv.f[3], v.w, E);
        else
            return DirectX::XMVector4NearEqual(mv, v.mv, DirectX::XMVectorReplicate(E));
    }

    constexpr inline T magnitude_sqr()
    {
        if (std::is_constant_evaluated())
            return cv.f[0] * cv.f[0] + cv.f[1] * cv.f[1] + cv.f[2] * cv.f[2] + cv.f[3] * cv.f[3];
        else
            return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(mv));
    }
    constexpr inline T magnitude()
    {
        if (std::is_constant_evaluated())
            return _sqrt(magnitude_sqr());
        else
            return DirectX::XMVectorGetX(DirectX::XMVector4Length(mv));
    }
    constexpr inline SelfRef normalize()
    {
        if (std::is_constant_evaluated())
            mul(1 / magnitude());
        else
            mv = DirectX::XMVector4Normalize(mv);

        return *this;
    }

    constexpr inline SelfRef normalize_as_plane()
    {
        if (std::is_constant_evaluated())
            mul(1 / _sqrt(cv.f[0] * cv.f[0] + cv.f[1] * cv.f[1] + cv.f[2] * cv.f[2]));
        else
        {
            DirectX::XMVECTOR tmp = DirectX::XMVector3ReciprocalLength(mv);
            mv = DirectX::XMVectorMultiply(mv, tmp);
        }

        return *this;
    }

    constexpr inline SelfRef lerp(const Self& p1, const Self& p2, T t)
    {
        if (std::is_constant_evaluated())
        {
            const T invt = 1.f - t;
            cv.f[0] = p1.x * invt + p2.x * t;
            cv.f[1] = p1.y * invt + p2.y * t;
            cv.f[2] = p1.z * invt + p2.z * t;
            cv.f[3] = p1.w * invt + p2.w * t;
        }
        else
            mv = DirectX::XMVectorLerp(p1.mv, p2.mv, t);

        return *this;
    }
};

typedef _fvector4<float> Fvector4;
static_assert(sizeof(Fvector4) == 16);

typedef _ivector4<s32> Ivector4;
static_assert(sizeof(Ivector4) == 16);

#endif /* __XR_CORE_VECTOR4_H */
