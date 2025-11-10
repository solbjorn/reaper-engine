#pragma once

template <typename T>
class XR_TRIVIAL _box2
{
public:
    typedef T TYPE;
    typedef _box2<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;
    typedef _vector2<T> Tvector;

public:
    union
    {
        struct
        {
            Tvector min;
            Tvector max;
        };
        struct
        {
            T x1, y1;
            T x2, y2;
        };
    };

    constexpr _box2() = default;
    constexpr explicit _box2(const Tvector& _min, const Tvector& _max) { set(_min, _max); }
    constexpr explicit _box2(T x1, T y1, T x2, T y2) { set(x1, y1, x2, y2); }

    constexpr _box2(const Self& that) { set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _box2(Self&&) = default;
#else
    constexpr _box2(Self&& that) { set(that); }
#endif

    constexpr Self& operator=(const Self& that) { return set(that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr Self& operator=(Self&&) = default;
#else
    constexpr Self& operator=(Self&& that) { return set(that); }
#endif

    constexpr SelfRef set(const Tvector& _min, const Tvector& _max)
    {
        min.set(_min);
        max.set(_max);
        return *this;
    }

    constexpr SelfRef set(T x1, T y1, T x2, T y2)
    {
        min.set(x1, y1);
        max.set(x2, y2);
        return *this;
    }

    constexpr SelfRef set(SelfCRef b)
    {
        min.set(b.min);
        max.set(b.max);
        return *this;
    }

    constexpr SelfRef null()
    {
        min.set(0.f, 0.f);
        max.set(0.f, 0.f);
        return *this;
    }

    constexpr SelfRef identity()
    {
        min.set(-0.5, -0.5, -0.5);
        max.set(0.5, 0.5, 0.5);
        return *this;
    }

    constexpr SelfRef invalidate()
    {
        min.set(type_max(T), type_max(T));
        max.set(type_min(T), type_min(T));
        return *this;
    }

    constexpr SelfRef shrink(T s)
    {
        min.add(s);
        max.sub(s);
        return *this;
    }

    constexpr SelfRef shrink(const Tvector& s)
    {
        min.add(s);
        max.sub(s);
        return *this;
    }

    constexpr SelfRef grow(T s)
    {
        min.sub(s);
        max.add(s);
        return *this;
    }

    constexpr SelfRef grow(const Tvector& s)
    {
        min.sub(s);
        max.add(s);
        return *this;
    }

    constexpr SelfRef add(const Tvector& p)
    {
        min.add(p);
        max.add(p);
        return *this;
    }

    constexpr SelfRef offset(const Tvector& p)
    {
        min.add(p);
        max.add(p);
        return *this;
    }

    constexpr SelfRef add(SelfCRef b, const Tvector& p)
    {
        min.add(b.min, p);
        max.add(b.max, p);
        return *this;
    }

    constexpr BOOL contains(T x, T y) { return (x >= x1) && (x <= x2) && (y >= y1) && (y <= y2); }
    constexpr BOOL contains(const Tvector& p) { return contains(p.x, p.y); }
    constexpr BOOL contains(SelfCRef b) { return contains(b.min) && contains(b.max); }

    constexpr BOOL similar(SelfCRef b) { return min.similar(b.min) && max.similar(b.max); }

    constexpr SelfRef modify(const Tvector& p)
    {
        min.min(p);
        max.max(p);
        return *this;
    }

    constexpr SelfRef merge(SelfCRef b)
    {
        modify(b.min);
        modify(b.max);
        return *this;
    }

    constexpr SelfRef merge(SelfCRef b1, SelfCRef b2)
    {
        invalidate();
        merge(b1);
        merge(b2);
        return *this;
    }

    constexpr void getsize(Tvector& R) const { R.sub(max, min); }

    constexpr void getradius(Tvector& R) const
    {
        getsize(R);
        R.mul(0.5f);
    }

    constexpr T getradius() const
    {
        Tvector R;
        getsize(R);
        R.mul(0.5f);
        return R.magnitude();
    }

    constexpr void getcenter(Tvector& C) const
    {
        C.x = (min.x + max.x) * 0.5f;
        C.y = (min.y + max.y) * 0.5f;
    }

    constexpr void getsphere(Tvector& C, T& R) const
    {
        getcenter(C);
        R = C.distance_to(max);
    }

    // Detects if this box intersect other
    constexpr BOOL intersect(SelfCRef box)
    {
        if (max.x < box.min.x)
            return FALSE;
        if (max.y < box.min.y)
            return FALSE;
        if (min.x > box.max.x)
            return FALSE;
        if (min.y > box.max.y)
            return FALSE;
        return TRUE;
    }

    // Make's this box valid AABB
    constexpr SelfRef sort()
    {
        T tmp;
        if (min.x > max.x)
        {
            tmp = min.x;
            min.x = max.x;
            max.x = tmp;
        }
        if (min.y > max.y)
        {
            tmp = min.y;
            min.y = max.y;
            max.y = tmp;
        }
        return *this;
    }

    // Does the vector3 intersects box
    constexpr BOOL Pick(const Tvector& start, const Tvector& dir)
    {
        T alpha, xt, yt;
        Tvector rvmin, rvmax;

        rvmin.sub(min, start);
        rvmax.sub(max, start);

        if (!fis_zero(dir.x))
        {
            alpha = rvmin.x / dir.x;
            yt = alpha * dir.y;
            if (yt >= rvmin.y && yt <= rvmax.y)
                return true;
            alpha = rvmax.x / dir.x;
            yt = alpha * dir.y;
            if (yt >= rvmin.y && yt <= rvmax.y)
                return true;
        }

        if (!fis_zero(dir.y))
        {
            alpha = rvmin.y / dir.y;
            xt = alpha * dir.x;
            if (xt >= rvmin.x && xt <= rvmax.x)
                return true;
            alpha = rvmax.y / dir.y;
            xt = alpha * dir.x;
            if (xt >= rvmin.x && xt <= rvmax.x)
                return true;
        }
        return false;
    }

    constexpr BOOL pick_exact(const Tvector& start, const Tvector& dir)
    {
        T alpha, xt, yt;
        Tvector rvmin, rvmax;

        rvmin.sub(min, start);
        rvmax.sub(max, start);

        if (!fis_zero(dir.x))
        {
            alpha = rvmin.x / dir.x;
            yt = alpha * dir.y;
            if (yt >= rvmin.y - EPS && yt <= rvmax.y + EPS)
                return true;
            alpha = rvmax.x / dir.x;
            yt = alpha * dir.y;
            if (yt >= rvmin.y - EPS && yt <= rvmax.y + EPS)
                return true;
        }

        if (!fis_zero(dir.y))
        {
            alpha = rvmin.y / dir.y;
            xt = alpha * dir.x;
            if (xt >= rvmin.x - EPS && xt <= rvmax.x + EPS)
                return true;
            alpha = rvmax.y / dir.y;
            xt = alpha * dir.x;
            if (xt >= rvmin.x - EPS && xt <= rvmax.x + EPS)
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr u32 IR(const T& x) { return std::bit_cast<u32>(x); }

    constexpr BOOL Pick2(const Tvector& origin, const Tvector& dir, Tvector& coord)
    {
        BOOL Inside = TRUE;
        Tvector MaxT;
        MaxT.x = MaxT.y = -1.0f;

        // Find candidate planes.
        {
            if (origin[0] < min[0])
            {
                coord[0] = min[0];
                Inside = FALSE;
                if (IR(dir[0]))
                    MaxT[0] = (min[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
            }
            else if (origin[0] > max[0])
            {
                coord[0] = max[0];
                Inside = FALSE;
                if (IR(dir[0]))
                    MaxT[0] = (max[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
            }
        }
        {
            if (origin[1] < min[1])
            {
                coord[1] = min[1];
                Inside = FALSE;
                if (IR(dir[1]))
                    MaxT[1] = (min[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
            }
            else if (origin[1] > max[1])
            {
                coord[1] = max[1];
                Inside = FALSE;
                if (IR(dir[1]))
                    MaxT[1] = (max[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
            }
        }

        // Ray origin inside bounding box
        if (Inside)
        {
            coord = origin;
            return true;
        }

        // Get largest of the maxT's for final choice of intersection
        u32 WhichPlane = 0;
        if (MaxT[1] > MaxT[0])
            WhichPlane = 1;

        // Check final candidate actually inside box
        if (IR(MaxT[WhichPlane]) & 0x80000000)
            return false;

        if (0 == WhichPlane)
        {
            // 1
            coord[1] = origin[1] + MaxT[0] * dir[1];
            if ((coord[1] < min[1]) || (coord[1] > max[1]))
                return false;
            return true;
        }
        else
        {
            // 0
            coord[0] = origin[0] + MaxT[1] * dir[0];
            if ((coord[0] < min[0]) || (coord[0] > max[0]))
                return false;
            return true;
        }
    }

    constexpr void getpoint(int index, Tvector& result)
    {
        switch (index)
        {
        case 0: result.set(min.x, min.y); break;
        case 1: result.set(min.x, min.y); break;
        case 2: result.set(max.x, min.y); break;
        case 3: result.set(max.x, min.y); break;
        default: result.set(0.f, 0.f); break;
        }
    }

    constexpr void getpoints(Tvector* result)
    {
        result[0].set(min.x, min.y);
        result[1].set(min.x, min.y);
        result[2].set(max.x, min.y);
        result[3].set(max.x, min.y);
    }
};

using Fbox2 = _box2<f32>;
static_assert(sizeof(Fbox2) == 16);
XR_TRIVIAL_ASSERT(Fbox2);

template <typename T>
[[nodiscard]] constexpr bool _valid(const _box2<T>& c)
{
    return _valid(c.min) && _valid(c.max);
}
