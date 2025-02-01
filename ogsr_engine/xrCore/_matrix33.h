#ifndef __XR_CORE_MATRIX33_H
#define __XR_CORE_MATRIX33_H

template <class T>
struct _matrix33
{
public:
    typedef _matrix33<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;
    typedef _vector3<T> Tvector;

public:
    union
    {
        struct
        { // Direct definition
            T _11, _12, _13;
            T _21, _22, _23;
            T _31, _32, _33;
        };
        struct
        {
            Tvector i;
            Tvector j;
            Tvector k;
        };
        T m[3][3]; // Array
    };
    // Class members
    constexpr inline SelfRef set_rapid(const _matrix<T>& a)
    {
        m[0][0] = a.m[0][0];
        m[0][1] = a.m[0][1];
        m[0][2] = -a.m[0][2];
        m[1][0] = a.m[1][0];
        m[1][1] = a.m[1][1];
        m[1][2] = -a.m[1][2];
        m[2][0] = -a.m[2][0];
        m[2][1] = -a.m[2][1];
        m[2][2] = a.m[2][2];
        return *this;
    }
    constexpr inline SelfRef set(SelfCRef a)
    {
        if (sizeof(_11) == 4)
        {
            const u64* src = reinterpret_cast<const u64*>(&a._11);
            u64* dst = reinterpret_cast<u64*>(&_11);

            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            _33 = a._33;
        }
        else
            CopyMemory(this, &a, sizeof(*this));

        return *this;
    }
    constexpr inline SelfRef set(const _matrix<T>& a)
    {
        i = a.i;
        j = a.j;
        k = a.k;

        return *this;
    }
    constexpr inline SelfRef identity(void)
    {
        _11 = 1.f;
        _12 = 0.f;
        _13 = 0.f;
        _21 = 0.f;
        _22 = 1.f;
        _23 = 0.f;
        _31 = 0.f;
        _32 = 0.f;
        _33 = 1.f;
        return *this;
    }

    constexpr inline SelfRef transpose(SelfCRef matSource) // faster version of transpose
    {
        _11 = matSource._11;
        _12 = matSource._21;
        _13 = matSource._31;
        _21 = matSource._12;
        _22 = matSource._22;
        _23 = matSource._32;
        _31 = matSource._13;
        _32 = matSource._23;
        _33 = matSource._33;
        return *this;
    }
    constexpr inline SelfRef transpose(const _matrix<T>& matSource) // faster version of transpose
    {
        _11 = matSource._11;
        _12 = matSource._21;
        _13 = matSource._31;
        _21 = matSource._12;
        _22 = matSource._22;
        _23 = matSource._32;
        _31 = matSource._13;
        _32 = matSource._23;
        _33 = matSource._33;
        return *this;
    }
    constexpr inline SelfRef transpose() // self transpose - slower
    {
        _matrix33 a;
        a.set(this);
        transpose(a);
        return *this;
    }

    constexpr inline SelfRef MxM(SelfCRef M1, SelfCRef M2)
    {
        m[0][0] = (M1.m[0][0] * M2.m[0][0] + M1.m[0][1] * M2.m[1][0] + M1.m[0][2] * M2.m[2][0]);
        m[1][0] = (M1.m[1][0] * M2.m[0][0] + M1.m[1][1] * M2.m[1][0] + M1.m[1][2] * M2.m[2][0]);
        m[2][0] = (M1.m[2][0] * M2.m[0][0] + M1.m[2][1] * M2.m[1][0] + M1.m[2][2] * M2.m[2][0]);
        m[0][1] = (M1.m[0][0] * M2.m[0][1] + M1.m[0][1] * M2.m[1][1] + M1.m[0][2] * M2.m[2][1]);
        m[1][1] = (M1.m[1][0] * M2.m[0][1] + M1.m[1][1] * M2.m[1][1] + M1.m[1][2] * M2.m[2][1]);
        m[2][1] = (M1.m[2][0] * M2.m[0][1] + M1.m[2][1] * M2.m[1][1] + M1.m[2][2] * M2.m[2][1]);
        m[0][2] = (M1.m[0][0] * M2.m[0][2] + M1.m[0][1] * M2.m[1][2] + M1.m[0][2] * M2.m[2][2]);
        m[1][2] = (M1.m[1][0] * M2.m[0][2] + M1.m[1][1] * M2.m[1][2] + M1.m[1][2] * M2.m[2][2]);
        m[2][2] = (M1.m[2][0] * M2.m[0][2] + M1.m[2][1] * M2.m[1][2] + M1.m[2][2] * M2.m[2][2]);
        return *this;
    }

    constexpr inline SelfRef MTxM(SelfCRef M1, SelfCRef M2)
    {
        m[0][0] = (M1.m[0][0] * M2.m[0][0] + M1.m[1][0] * M2.m[1][0] + M1.m[2][0] * M2.m[2][0]);
        m[1][0] = (M1.m[0][1] * M2.m[0][0] + M1.m[1][1] * M2.m[1][0] + M1.m[2][1] * M2.m[2][0]);
        m[2][0] = (M1.m[0][2] * M2.m[0][0] + M1.m[1][2] * M2.m[1][0] + M1.m[2][2] * M2.m[2][0]);
        m[0][1] = (M1.m[0][0] * M2.m[0][1] + M1.m[1][0] * M2.m[1][1] + M1.m[2][0] * M2.m[2][1]);
        m[1][1] = (M1.m[0][1] * M2.m[0][1] + M1.m[1][1] * M2.m[1][1] + M1.m[2][1] * M2.m[2][1]);
        m[2][1] = (M1.m[0][2] * M2.m[0][1] + M1.m[1][2] * M2.m[1][1] + M1.m[2][2] * M2.m[2][1]);
        m[0][2] = (M1.m[0][0] * M2.m[0][2] + M1.m[1][0] * M2.m[1][2] + M1.m[2][0] * M2.m[2][2]);
        m[1][2] = (M1.m[0][1] * M2.m[0][2] + M1.m[1][1] * M2.m[1][2] + M1.m[2][1] * M2.m[2][2]);
        m[2][2] = (M1.m[0][2] * M2.m[0][2] + M1.m[1][2] * M2.m[1][2] + M1.m[2][2] * M2.m[2][2]);
        return *this;
    }

    //--------------------------------------------------------------------------------
    // other unused function
    //--------------------------------------------------------------------------------
    constexpr inline SelfRef McolcMcol(int cr, SelfCRef M, int c)
    {
        m[0][cr] = M.m[0][c];
        m[1][cr] = M.m[1][c];
        m[2][cr] = M.m[2][c];
        return *this;
    }

    constexpr inline SelfRef MxMpV(SelfCRef M1, SelfCRef M2, const Tvector& V)
    {
        m[0][0] = (M1.m[0][0] * M2.m[0][0] + M1.m[0][1] * M2.m[1][0] + M1.m[0][2] * M2.m[2][0] + V.x);
        m[1][0] = (M1.m[1][0] * M2.m[0][0] + M1.m[1][1] * M2.m[1][0] + M1.m[1][2] * M2.m[2][0] + V.y);
        m[2][0] = (M1.m[2][0] * M2.m[0][0] + M1.m[2][1] * M2.m[1][0] + M1.m[2][2] * M2.m[2][0] + V.z);
        m[0][1] = (M1.m[0][0] * M2.m[0][1] + M1.m[0][1] * M2.m[1][1] + M1.m[0][2] * M2.m[2][1] + V.x);
        m[1][1] = (M1.m[1][0] * M2.m[0][1] + M1.m[1][1] * M2.m[1][1] + M1.m[1][2] * M2.m[2][1] + V.y);
        m[2][1] = (M1.m[2][0] * M2.m[0][1] + M1.m[2][1] * M2.m[1][1] + M1.m[2][2] * M2.m[2][1] + V.z);
        m[0][2] = (M1.m[0][0] * M2.m[0][2] + M1.m[0][1] * M2.m[1][2] + M1.m[0][2] * M2.m[2][2] + V.x);
        m[1][2] = (M1.m[1][0] * M2.m[0][2] + M1.m[1][1] * M2.m[1][2] + M1.m[1][2] * M2.m[2][2] + V.y);
        m[2][2] = (M1.m[2][0] * M2.m[0][2] + M1.m[2][1] * M2.m[1][2] + M1.m[2][2] * M2.m[2][2] + V.z);
        return *this;
    }

    constexpr inline SelfRef Mqinverse(SelfCRef M)
    {
        int i, j;

        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
            {
                int i1 = (i + 1) % 3;
                int i2 = (i + 2) % 3;
                int j1 = (j + 1) % 3;
                int j2 = (j + 2) % 3;
                m[i][j] = (M.m[j1][i1] * M.m[j2][i2] - M.m[j1][i2] * M.m[j2][i1]);
            }
        return *this;
    }

    constexpr inline SelfRef MxMT(SelfCRef M1, SelfCRef M2)
    {
        m[0][0] = (M1.m[0][0] * M2.m[0][0] + M1.m[0][1] * M2.m[0][1] + M1.m[0][2] * M2.m[0][2]);
        m[1][0] = (M1.m[1][0] * M2.m[0][0] + M1.m[1][1] * M2.m[0][1] + M1.m[1][2] * M2.m[0][2]);
        m[2][0] = (M1.m[2][0] * M2.m[0][0] + M1.m[2][1] * M2.m[0][1] + M1.m[2][2] * M2.m[0][2]);
        m[0][1] = (M1.m[0][0] * M2.m[1][0] + M1.m[0][1] * M2.m[1][1] + M1.m[0][2] * M2.m[1][2]);
        m[1][1] = (M1.m[1][0] * M2.m[1][0] + M1.m[1][1] * M2.m[1][1] + M1.m[1][2] * M2.m[1][2]);
        m[2][1] = (M1.m[2][0] * M2.m[1][0] + M1.m[2][1] * M2.m[1][1] + M1.m[2][2] * M2.m[1][2]);
        m[0][2] = (M1.m[0][0] * M2.m[2][0] + M1.m[0][1] * M2.m[2][1] + M1.m[0][2] * M2.m[2][2]);
        m[1][2] = (M1.m[1][0] * M2.m[2][0] + M1.m[1][1] * M2.m[2][1] + M1.m[1][2] * M2.m[2][2]);
        m[2][2] = (M1.m[2][0] * M2.m[2][0] + M1.m[2][1] * M2.m[2][1] + M1.m[2][2] * M2.m[2][2]);
        return *this;
    }

    constexpr inline SelfRef MskewV(const Tvector& v)
    {
        m[0][0] = m[1][1] = m[2][2] = 0.0;
        m[1][0] = v.z;
        m[0][1] = -v.z;
        m[0][2] = v.y;
        m[2][0] = -v.y;
        m[1][2] = -v.x;
        m[2][1] = v.x;
        return *this;
    }
    constexpr inline SelfRef sMxVpV(Tvector& R, float s1, const Tvector& V1, const Tvector& V2) const
    {
        R.x = s1 * (m[0][0] * V1.x + m[0][1] * V1.y + m[0][2] * V1.z) + V2.x;
        R.y = s1 * (m[1][0] * V1.x + m[1][1] * V1.y + m[1][2] * V1.z) + V2.y;
        R.z = s1 * (m[2][0] * V1.x + m[2][1] * V1.y + m[2][2] * V1.z) + V2.z;
        return *this;
    }
    constexpr inline void MTxV(Tvector& R, const Tvector& V1) const
    {
        R.x = (m[0][0] * V1.x + m[1][0] * V1.y + m[2][0] * V1.z);
        R.y = (m[0][1] * V1.x + m[1][1] * V1.y + m[2][1] * V1.z);
        R.z = (m[0][2] * V1.x + m[1][2] * V1.y + m[2][2] * V1.z);
    }
    constexpr inline void MTxVpV(Tvector& R, const Tvector& V1, const Tvector& V2) const
    {
        R.x = (m[0][0] * V1.x + m[1][0] * V1.y + m[2][0] * V1.z + V2.x);
        R.y = (m[0][1] * V1.x + m[1][1] * V1.y + m[2][1] * V1.z + V2.y);
        R.z = (m[0][2] * V1.x + m[1][2] * V1.y + m[2][2] * V1.z + V2.z);
    }
    constexpr inline SelfRef MTxVmV(Tvector& R, const Tvector& V1, const Tvector& V2) const
    {
        R.x = (m[0][0] * V1.x + m[1][0] * V1.y + m[2][0] * V1.z - V2.x);
        R.y = (m[0][1] * V1.x + m[1][1] * V1.y + m[2][1] * V1.z - V2.y);
        R.z = (m[0][2] * V1.x + m[1][2] * V1.y + m[2][2] * V1.z - V2.z);
        return *this;
    }
    constexpr inline SelfRef sMTxV(Tvector& R, float s1, const Tvector& V1) const
    {
        R.x = s1 * (m[0][0] * V1.x + m[1][0] * V1.y + m[2][0] * V1.z);
        R.y = s1 * (m[0][1] * V1.x + m[1][1] * V1.y + m[2][1] * V1.z);
        R.z = s1 * (m[0][2] * V1.x + m[1][2] * V1.y + m[2][2] * V1.z);
        return *this;
    }
    constexpr inline SelfRef MxV(Tvector& R, const Tvector& V1) const
    {
        R.x = (m[0][0] * V1.x + m[0][1] * V1.y + m[0][2] * V1.z);
        R.y = (m[1][0] * V1.x + m[1][1] * V1.y + m[1][2] * V1.z);
        R.z = (m[2][0] * V1.x + m[2][1] * V1.y + m[2][2] * V1.z);
        return *this;
    }
    constexpr inline void transform_dir(_vector2<T>& dest, const _vector2<T>& v) const // preferred to use
    {
        dest.x = v.x * _11 + v.y * _21;
        dest.y = v.x * _12 + v.y * _22;
        dest.z = v.x * _13 + v.y * _23;
    }
    constexpr inline void transform_dir(_vector2<T>& v) const
    {
        _vector2<T> res;
        transform_dir(res, v);
        v.set(res);
    }
    constexpr inline SelfRef MxVpV(Tvector& R, const Tvector& V1, const Tvector& V2) const
    {
        R.x = (m[0][0] * V1.x + m[0][1] * V1.y + m[0][2] * V1.z + V2.x);
        R.y = (m[1][0] * V1.x + m[1][1] * V1.y + m[1][2] * V1.z + V2.y);
        R.z = (m[2][0] * V1.x + m[2][1] * V1.y + m[2][2] * V1.z + V2.z);
        return *this;
    }
};

typedef _matrix33<float> Fmatrix33;
static_assert(sizeof(Fmatrix33) == 36);

typedef _matrix33<double> Dmatrix33;

template <class T>
BOOL _valid(const _matrix33<T>& m)
{
    return _valid(m.i) && _valid(m.j) && _valid(m.k);
}

#endif /* __XR_CORE_MATRIX33_H */
