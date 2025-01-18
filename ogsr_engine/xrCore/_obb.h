#pragma once

template <class T>
struct _obb
{
public:
    typedef _obb<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;
    typedef _vector3<T> Tvector;
    typedef _matrix<T> Tmatrix;

protected:
    static bool clip(T fDenom, T fNumer, T& rfT0, T& rfT1)
    {
        // Return value is 'true' if line segment intersects the current test
        // plane.  Otherwise 'false' is returned in which case the line segment
        // is entirely clipped.

        if (fDenom > 0.0f)
        {
            if (fNumer > fDenom * rfT1)
                return false;
            if (fNumer > fDenom * rfT0)
                rfT0 = fNumer / fDenom;
            return true;
        }
        else if (fDenom < 0.0f)
        {
            if (fNumer > fDenom * rfT0)
                return false;
            if (fNumer > fDenom * rfT1)
                rfT1 = fNumer / fDenom;
            return true;
        }
        else
        {
            return fNumer <= 0.0f;
        }
    }
    static bool intersect(const Tvector& start, const Tvector& dir, const Tvector& extent, T& rfT0, T& rfT1)
    {
        T fSaveT0 = rfT0, fSaveT1 = rfT1;

        bool bNotEntirelyClipped = clip(+dir.x, -start.x - extent[0], rfT0, rfT1) && clip(-dir.x, +start.x - extent[0], rfT0, rfT1) &&
            clip(+dir.y, -start.y - extent[1], rfT0, rfT1) && clip(-dir.y, +start.y - extent[1], rfT0, rfT1) && clip(+dir.z, -start.z - extent[2], rfT0, rfT1) &&
            clip(-dir.z, +start.z - extent[2], rfT0, rfT1);

        return bNotEntirelyClipped && (rfT0 != fSaveT0 || rfT1 != fSaveT1);
    }

public:
    _matrix33<T> m_rotate;
    Tvector m_translate;
    Tvector m_halfsize;

    IC SelfRef invalidate()
    {
        m_rotate.identity();
        m_translate.set(0, 0, 0);
        m_halfsize.set(0, 0, 0);
        return *this;
    }
    IC SelfRef identity()
    {
        invalidate();
        m_halfsize.set(T(0.5), T(0.5), T(0.5));
        return *this;
    }
    IC void xform_get(Tmatrix& D) const
    {
        D.vm[0].set(m_rotate.i, 0);
        D.vm[1].set(m_rotate.j, 0);
        D.vm[2].set(m_rotate.k, 0);
        D.vm[3].set(m_translate, 1);
    }
    IC SelfRef xform_set(const Tmatrix& S)
    {
        DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&m_rotate.i), *reinterpret_cast<const DirectX::XMVECTOR*>(&S.vm[0]));
        DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&m_rotate.j), *reinterpret_cast<const DirectX::XMVECTOR*>(&S.vm[1]));
        DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&m_rotate.k), *reinterpret_cast<const DirectX::XMVECTOR*>(&S.vm[2]));
        DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&m_translate), *reinterpret_cast<const DirectX::XMVECTOR*>(&S.vm[3]));
        return *this;
    }
    IC void xform_full(Tmatrix& D) const
    {
        Tmatrix R, S;
        xform_get(R);
        S.scale(m_halfsize);
        D.mul_43(R, S);
    }

    // NOTE: Unoptimized
    IC SelfRef transform(SelfCRef src, const Tmatrix& M)
    {
        Tmatrix srcR, destR;

        src.xform_get(srcR);
        destR.mul_43(M, srcR);
        xform_set(destR);
        m_halfsize.set(src.m_halfsize);
        return *this;
    }

    IC bool intersect(const Tvector& start, const Tvector& dir, T& dist) const
    {
        // convert ray to box coordinates
        Tvector kDiff;
        kDiff.sub(start, m_translate);
        Tvector kOrigin;
        kOrigin.set(kDiff.dotproduct(m_rotate.i), kDiff.dotproduct(m_rotate.j), kDiff.dotproduct(m_rotate.k));
        Tvector kDirection;
        kDirection.set(dir.dotproduct(m_rotate.i), dir.dotproduct(m_rotate.j), dir.dotproduct(m_rotate.k));

        T fT0 = 0.0f, fT1 = type_max(T);
        if (intersect(kOrigin, kDirection, m_halfsize, fT0, fT1))
        {
            bool bPick = false;
            if (fT0 > 0.0f)
            {
                if (fT0 < dist)
                {
                    dist = fT0;
                    bPick = true;
                }
                if (fT1 < dist)
                {
                    dist = fT1;
                    bPick = true;
                }
            }
            else
            {
                if (fT1 < dist)
                {
                    dist = fT1;
                    bPick = true;
                }
            }
            return bPick;
        }
        return false;
    }
};

typedef _obb<float> Fobb;
typedef _obb<double> Dobb;

template <class T>
BOOL _valid(const _obb<T>& m)
{
    return _valid(m.m_rotate) && _valid(m.m_translate) && _valid(m.m_halfsize);
}
