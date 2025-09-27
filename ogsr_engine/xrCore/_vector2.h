#ifndef __XR_CORE_VECTOR2_H
#define __XR_CORE_VECTOR2_H

template <class T>
struct alignas(8) _vector2
{
public:
    typedef T TYPE;
    typedef _vector2<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;

public:
    T x, y;

    constexpr inline SelfRef set(float _u, float _v)
    {
        x = T(_u);
        y = T(_v);
        return *this;
    }
    constexpr inline SelfRef set(int _u, int _v)
    {
        x = T(_u);
        y = T(_v);
        return *this;
    }
    constexpr inline SelfRef set(const Self& p)
    {
        *reinterpret_cast<u64*>(&x) = *reinterpret_cast<const u64*>(&p.x);
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
    constexpr inline SelfRef sub(const Self& p, float d)
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
    constexpr inline SelfRef add(const Self& p, float d)
    {
        x = p.x + d;
        y = p.y + d;
        return *this;
    }
    constexpr inline SelfRef mul(const T s)
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
    constexpr inline SelfRef rot90(void)
    {
        float t = -x;
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
    constexpr inline T dot(Self& p) { return x * p.x + y * p.y; }
    constexpr inline T dot(const Self& p) const { return x * p.x + y * p.y; }
    constexpr inline SelfRef norm(void)
    {
        float m = _sqrt(x * x + y * y);
        x /= m;
        y /= m;
        return *this;
    }
    constexpr inline SelfRef norm_safe(void)
    {
        float m = _sqrt(x * x + y * y);
        if (m)
        {
            x /= m;
            y /= m;
        }
        return *this;
    }
    constexpr inline T distance_to(const Self& p) const { return _sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y)); }
    constexpr inline T square_magnitude(void) const { return x * x + y * y; }
    constexpr inline T magnitude(void) const { return _sqrt(square_magnitude()); }

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

    constexpr inline bool similar(Self& p, T eu, T ev) const
    {
        if constexpr (std::is_floating_point_v<T>)
            return fsimilar(x, p.x, eu) && fsimilar(y, p.y, ev);
        else
            return x == p.x && y == p.y;
    }

    constexpr inline bool similar(const Self& p, T E = EPS_L) const
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

    constexpr inline T& operator[](int i) const
    {
        // assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes
        return (T&)*(&x + i);
    }

    constexpr inline SelfRef normalize(void) { return norm(); }
    constexpr inline SelfRef normalize_safe(void) { return norm_safe(); }
    constexpr inline SelfRef normalize(const Self& v)
    {
        float m = _sqrt(v.x * v.x + v.y * v.y);
        x = v.x / m;
        y = v.y / m;
        return *this;
    }
    constexpr inline SelfRef normalize_safe(const Self& v)
    {
        float m = _sqrt(v.x * v.x + v.y * v.y);
        if (m)
        {
            x = v.x / m;
            y = v.y / m;
        }
        return *this;
    }
    constexpr inline float dotproduct(const Self& p) const { return dot(p); }
    constexpr inline float crossproduct(const Self& p) const { return y * p.x - x * p.y; }
    constexpr inline float getH(void) const
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

typedef _vector2<float> Fvector2;
static_assert(sizeof(Fvector2) == 8);

typedef _vector2<int> Ivector2;
static_assert(sizeof(Ivector2) == 8);

#endif /* __XR_CORE_VECTOR2_H */
