#ifndef __XR_CORE_VECTOR2_H
#define __XR_CORE_VECTOR2_H

template <typename T>
struct XR_TRIVIAL alignas(8) _vector2
{
public:
    typedef T TYPE;
    typedef _vector2<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;

    union
    {
        struct
        {
            T x, y;
        };
        std::array<T, 2> arr;
    };

    constexpr _vector2() = default;
    constexpr explicit _vector2(T _x, T _y) : x{_x}, y{_y} {}

    constexpr _vector2(const Self& that) { set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _vector2(Self&&) = default;
#else
    constexpr _vector2(Self&& that) { set(that); }
#endif

    constexpr Self& operator=(const Self& that) { return set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr Self& operator=(Self&&) = default;
#else
    constexpr Self& operator=(Self&& that) { return set(that); }
#endif

    constexpr SelfRef set(T _u, T _v)
    {
        x = _u;
        y = _v;

        return *this;
    }

    constexpr SelfRef set(const Self& that)
    {
        xr::memcpy8(this, &that);
        return *this;
    }

    constexpr inline SelfRef abs(const Self& p)
    {
        x = _abs(p.x);
        y = _abs(p.y);
        return *this;
    }

    constexpr inline SelfRef min(const Self& p)
    {
        x = _min(x, p.x);
        y = _min(y, p.y);
        return *this;
    }

    constexpr inline SelfRef min(T _x, T _y)
    {
        x = _min(x, _x);
        y = _min(y, _y);
        return *this;
    }

    constexpr inline SelfRef max(const Self& p)
    {
        x = _max(x, p.x);
        y = _max(y, p.y);
        return *this;
    }

    constexpr inline SelfRef max(T _x, T _y)
    {
        x = _max(x, _x);
        y = _max(y, _y);
        return *this;
    }

    constexpr inline SelfRef sub(const T p)
    {
        x -= p;
        y -= p;
        return *this;
    }

    constexpr inline SelfRef sub(const Self& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    constexpr inline SelfRef sub(const Self& p1, const Self& p2)
    {
        x = p1.x - p2.x;
        y = p1.y - p2.y;
        return *this;
    }

    constexpr inline SelfRef sub(const Self& p, T d)
    {
        x = p.x - d;
        y = p.y - d;
        return *this;
    }

    constexpr inline SelfRef add(const T p)
    {
        x += p;
        y += p;
        return *this;
    }

    constexpr inline SelfRef add(const Self& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    constexpr inline SelfRef add(const Self& p1, const Self& p2)
    {
        x = p1.x + p2.x;
        y = p1.y + p2.y;
        return *this;
    }

    constexpr inline SelfRef add(const Self& p, T d)
    {
        x = p.x + d;
        y = p.y + d;
        return *this;
    }

    constexpr inline SelfRef mul(T s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    constexpr inline SelfRef mul(const Self& p)
    {
        x *= p.x;
        y *= p.y;
        return *this;
    }

    constexpr inline SelfRef div(const T s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    constexpr inline SelfRef div(const Self& p)
    {
        x /= p.x;
        y /= p.y;
        return *this;
    }

    constexpr SelfRef rot90()
    {
        T t = -x;
        x = y;
        y = t;

        return *this;
    }

    constexpr inline SelfRef cross(const Self& D)
    {
        x = D.y;
        y = -D.x;
        return *this;
    }

    [[nodiscard]] constexpr T dot(const Self& p) const { return x * p.x + y * p.y; }

    constexpr SelfRef norm()
    {
        T m = _sqrt(x * x + y * y);
        x /= m;
        y /= m;

        return *this;
    }

    constexpr SelfRef norm_safe()
    {
        T m = _sqrt(x * x + y * y);
        if (!fis_zero(m))
        {
            x /= m;
            y /= m;
        }

        return *this;
    }

    [[nodiscard]] constexpr T distance_to(const Self& p) const { return _sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y)); }
    [[nodiscard]] constexpr T square_magnitude() const { return x * x + y * y; }
    [[nodiscard]] constexpr T magnitude() const { return _sqrt(square_magnitude()); }

    constexpr inline SelfRef mad(const Self& p, const Self& d, T r)
    {
        x = p.x + d.x * r;
        y = p.y + d.y * r;
        return *this;
    }

    constexpr inline Self Cross()
    {
        // vector3 orthogonal to (x,y) is (y,-x)
        Self kCross;
        kCross.x = y;
        kCross.y = -x;
        return kCross;
    }

    [[nodiscard]] constexpr bool similar(Self& p, T eu, T ev) const
    {
        if constexpr (std::is_floating_point_v<T>)
            return fsimilar(x, p.x, eu) && fsimilar(y, p.y, ev);
        else
            return x == p.x && y == p.y;
    }

    [[nodiscard]] constexpr bool similar(const Self& p, T E = EPS_L) const
    {
        if constexpr (std::is_floating_point_v<T>)
            return fsimilar(x, p.x, E) && fsimilar(y, p.y, E);
        else
            return x == p.x && y == p.y;
    }

    // average arithmetic
    constexpr inline SelfRef averageA(Self& p1, Self& p2)
    {
        x = (p1.x + p2.x) * .5f;
        y = (p1.y + p2.y) * .5f;
        return *this;
    }

    // average geometric
    constexpr inline SelfRef averageG(Self& p1, Self& p2)
    {
        x = _sqrt(p1.x * p2.x);
        y = _sqrt(p1.y * p2.y);
        return *this;
    }

    [[nodiscard]] constexpr T& operator[](gsl::index i)
    {
        // assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes
        return arr[gsl::narrow_cast<size_t>(i)];
    }

    [[nodiscard]] constexpr const T& operator[](gsl::index i) const
    {
        // assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes
        return arr[gsl::narrow_cast<size_t>(i)];
    }

    constexpr SelfRef normalize() { return norm(); }
    constexpr SelfRef normalize_safe() { return norm_safe(); }

    constexpr SelfRef normalize(const Self& v)
    {
        T m = _sqrt(v.x * v.x + v.y * v.y);
        x = v.x / m;
        y = v.y / m;

        return *this;
    }

    constexpr SelfRef normalize_safe(const Self& v)
    {
        T m = _sqrt(v.x * v.x + v.y * v.y);
        if (!fis_zero(m))
        {
            x = v.x / m;
            y = v.y / m;
        }

        return *this;
    }

    [[nodiscard]] constexpr T dotproduct(const Self& p) const { return dot(p); }
    [[nodiscard]] constexpr T crossproduct(const Self& p) const { return y * p.x - x * p.y; }

    [[nodiscard]] constexpr T getH() const
    {
        if (fis_zero(y))
            if (fis_zero(x))
                return (0.f);
            else
                return ((x > 0.0f) ? -PI_DIV_2 : PI_DIV_2);
        else if (y < 0.f)
            return (-(atanf(x / y) - PI));
        else
            return (-atanf(x / y));
    }
};

using Fvector2 = _vector2<f32>;
static_assert(sizeof(Fvector2) == 8);
XR_TRIVIAL_ASSERT(Fvector2);

using Ivector2 = _vector2<s32>;
static_assert(sizeof(Ivector2) == 8);
XR_TRIVIAL_ASSERT(Ivector2);

template <typename T>
[[nodiscard]] constexpr bool _valid(_vector2<T> v)
{
    return _valid(v.x) && _valid(v.y);
}

#endif /* __XR_CORE_VECTOR2_H */
