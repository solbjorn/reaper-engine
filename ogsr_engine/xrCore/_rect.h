#pragma once

template <typename T>
struct XR_TRIVIAL _rect
{
public:
    typedef T TYPE;
    typedef _rect<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;
    typedef _vector2<T> Tvector;

public:
    union
    {
        struct
        {
            T x1, y1, x2, y2;
        };
        struct
        {
            T left, top, right, bottom;
        };

        struct
        {
            Tvector lt;
            Tvector rb;
        };
        T m[4];
    };

    constexpr _rect() = default;
    constexpr explicit _rect(T _x1, T _y1, T _x2, T _y2) : x1{_x1}, y1{_y1}, x2{_x2}, y2{_y2} {}
    constexpr explicit _rect(const Tvector& mn, const Tvector& mx) { set(mn, mx); }

    constexpr _rect(const Self& that) { set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _rect(Self&&) = default;
#else
    constexpr _rect(Self&& that) { set(that); }
#endif

    constexpr Self& operator=(const Self& that) { return set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr Self& operator=(Self&&) = default;
#else
    constexpr Self& operator=(Self&& that) { return set(that); }
#endif

    constexpr SelfRef set(T _x1, T _y1, T _x2, T _y2)
    {
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
        return *this;
    }

    constexpr SelfRef set(const Tvector& mn, const Tvector& mx)
    {
        x1 = mn.x;
        y1 = mn.y;
        x2 = mx.x;
        y2 = mx.y;
        return *this;
    }

    constexpr SelfRef set(const Self& r)
    {
        x1 = r.x1;
        y1 = r.y1;
        x2 = r.x2;
        y2 = r.y2;
        return *this;
    }

    constexpr SelfRef null()
    {
        x1 = T(0);
        y1 = T(0);
        x2 = T(0);
        y2 = T(0);
        return *this;
    }

    constexpr SelfRef invalidate()
    {
        lt.x = type_max(T);
        lt.y = type_max(T);
        rb.x = type_min(T);
        rb.y = type_min(T);
        return *this;
    }

    constexpr bool valide() const { return lt.x1 < rb.x && lt.y < rb.y; }
    constexpr SelfRef set_empty() { return invalidate(); }
    constexpr bool is_empty() const { return !valide(); }

    constexpr SelfRef add(T x, T y)
    {
        x1 += x;
        y1 += y;
        x2 += x;
        y2 += y;
        return *this;
    }

    constexpr SelfRef sub(T x, T y)
    {
        x1 -= x;
        y1 -= y;
        x2 -= x;
        y2 -= y;
        return *this;
    }

    constexpr SelfRef mul(T x, T y)
    {
        x1 *= x;
        y1 *= y;
        x2 *= x;
        y2 *= y;
        return *this;
    }

    constexpr SelfRef div(T x, T y)
    {
        x1 /= x;
        y1 /= y;
        x2 /= x;
        y2 /= y;
        return *this;
    }

    constexpr SelfRef add(const Self& r, T x, T y)
    {
        x1 = r.x1 + x;
        y1 = r.y1 + y;
        x2 = r.x2 + x;
        y2 = r.y2 + y;
        return *this;
    }

    constexpr SelfRef sub(const Self& r, T x, T y)
    {
        x1 = r.x1 - x;
        y1 = r.y1 - y;
        x2 = r.x2 - x;
        y2 = r.y2 - y;
        return *this;
    }

    constexpr SelfRef mul(const Self& r, T x, T y)
    {
        x1 = r.x1 * x;
        y1 = r.y1 * y;
        x2 = r.x2 * x;
        y2 = r.y2 * y;
        return *this;
    }

    constexpr SelfRef div(const Self& r, T x, T y)
    {
        x1 = r.x1 / x;
        y1 = r.y1 / y;
        x2 = r.x2 / x;
        y2 = r.y2 / y;
        return *this;
    }

    constexpr BOOL in(T x, T y) const { return (x >= x1) && (x <= x2) && (y >= y1) && (y <= y2); }
    constexpr BOOL in(const Tvector& p) const { return (p.x >= x1) && (p.x <= x2) && (p.y >= y1) && (p.y <= y2); }
    constexpr BOOL cmp(const _rect<int>& r) const { return x1 == r.x1 && y1 == r.y1 && x2 == r.x2 && y2 == r.y2; }
    constexpr BOOL cmp(const _rect<float>& r) const { return fsimilar(x1, r.x1) && fsimilar(y1, r.y1) && fsimilar(x2, r.x2) && fsimilar(y2, r.y2); }

    constexpr void getcenter(Tvector& center) const
    {
        center.add(rb, lt);
        center.div(2);
    }

    constexpr void getsize(Tvector& sz) const { sz.sub(rb, lt); }

    constexpr T width() const { return rb.x - lt.x; }
    constexpr T height() const { return rb.y - lt.y; }

    constexpr SelfRef shrink(T x, T y)
    {
        lt.x += x;
        lt.y += y;
        rb.x -= x;
        rb.y -= y;
        return *this;
    }

    constexpr SelfRef grow(T x, T y)
    {
        lt.x -= x;
        lt.y -= y;
        rb.x += x;
        rb.y += y;
        return *this;
    }

    constexpr BOOL intersected(SelfCRef b1, SelfCRef b2) const { return !(b1.x1 > b2.x2 || b1.x2 < b2.x1 || b1.y1 > b2.y2 || b1.y2 < b2.y1); }
    constexpr BOOL intersected(SelfCRef b) const { return intersected(*this, b); }

    constexpr BOOL intersection(SelfCRef b1, SelfCRef b2)
    {
        if (!intersected(b1, b2))
            return (FALSE);

        x1 = _max(b1.x1, b2.x1);
        y1 = _max(b1.y1, b2.y1);
        x2 = _min(b1.x2, b2.x2);
        y2 = _min(b1.y2, b2.y2);
        return (TRUE);
    }
};

using Frect = _rect<f32>;
static_assert(sizeof(Frect) == 16);
XR_TRIVIAL_ASSERT(Frect);

using Irect = _rect<s32>;
static_assert(sizeof(Irect) == 16);
XR_TRIVIAL_ASSERT(Irect);

template <typename T>
[[nodiscard]] constexpr bool _valid(const _rect<T>& m)
{
    return m.lt._valid() && m.rb._valid();
}
