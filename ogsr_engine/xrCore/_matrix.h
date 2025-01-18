#ifndef __XR_CORE_MATRIX_H
#define __XR_CORE_MATRIX_H

#include <DirectXMath.h>

/*
 *	DirectX-compliant, ie row-column order, ie m[Row][Col].
 *	Same as:
 *	m11  m12  m13  m14	first row.
 *	m21  m22  m23  m24	second row.
 *	m31  m32  m33  m34	third row.
 *	m41  m42  m43  m44	fourth row.
 *	Translation is (m41, m42, m43), (m14, m24, m34, m44) = (0, 0, 0, 1).
 *	Stored in memory as m11 m12 m13 m14 m21...
 *
 *	Multiplication rules:
 *
 *	[x'y'z'1] = [xyz1][M]
 *
 *	x' = x*m11 + y*m21 + z*m31 + m41
 *	y' = x*m12 + y*m22 + z*m32 + m42
 *	z' = x*m13 + y*m23 + z*m33 + m43
 *	1' =     0 +     0 +     0 + m44
 */

// NOTE_1: positive angle means clockwise rotation
// NOTE_2: mul(A,B) means transformation B, followed by A
// NOTE_3: I,J,K,C equals to R,N,D,T
// NOTE_4: The rotation sequence is ZXY

template <class T>
struct alignas(16) _matrix
{
public:
    typedef T TYPE;
    typedef _matrix<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;
    typedef _vector3<T> Tvector;

public:
    union
    {
        DirectX::XMFLOAT4X4A cm;
        DirectX::XMMATRIX mm;
        Fvector4 vm[4];

        struct
        {
            Tvector i;
            T _unused_14_;
            Tvector j;
            T _unused_24_;
            Tvector k;
            T _unused_34_;
            Tvector c;
            T _unused_44_;
        };
    };

    /* mm.r[0] */
    constexpr inline T get_11() const
    {
        if (std::is_constant_evaluated())
            return cm._11;
        else
            return DirectX::XMVectorGetX(mm.r[0]);
    }
    constexpr inline T get_12() const
    {
        if (std::is_constant_evaluated())
            return cm._12;
        else
            return DirectX::XMVectorGetY(mm.r[0]);
    }
    constexpr inline T get_13() const
    {
        if (std::is_constant_evaluated())
            return cm._13;
        else
            return DirectX::XMVectorGetZ(mm.r[0]);
    }
    constexpr inline T get_14() const
    {
        if (std::is_constant_evaluated())
            return cm._14;
        else
            return DirectX::XMVectorGetW(mm.r[0]);
    }
    constexpr inline void set_11(T x)
    {
        if (std::is_constant_evaluated())
            cm._11 = x;
        else
            mm.r[0] = DirectX::XMVectorSetX(mm.r[0], x);
    }
    constexpr inline void set_12(T y)
    {
        if (std::is_constant_evaluated())
            cm._12 = y;
        else
            mm.r[0] = DirectX::XMVectorSetY(mm.r[0], y);
    }
    constexpr inline void set_13(T z)
    {
        if (std::is_constant_evaluated())
            cm._13 = z;
        else
            mm.r[0] = DirectX::XMVectorSetZ(mm.r[0], z);
    }
    constexpr inline void set_14(T w)
    {
        if (std::is_constant_evaluated())
            cm._14 = w;
        else
            mm.r[0] = DirectX::XMVectorSetW(mm.r[0], w);
    }

    /* mm.r[1] */
    constexpr inline T get_21() const
    {
        if (std::is_constant_evaluated())
            return cm._21;
        else
            return DirectX::XMVectorGetX(mm.r[1]);
    }
    constexpr inline T get_22() const
    {
        if (std::is_constant_evaluated())
            return cm._22;
        else
            return DirectX::XMVectorGetY(mm.r[1]);
    }
    constexpr inline T get_23() const
    {
        if (std::is_constant_evaluated())
            return cm._23;
        else
            return DirectX::XMVectorGetZ(mm.r[1]);
    }
    constexpr inline T get_24() const
    {
        if (std::is_constant_evaluated())
            return cm._24;
        else
            return DirectX::XMVectorGetW(mm.r[1]);
    }
    constexpr inline void set_21(T x)
    {
        if (std::is_constant_evaluated())
            cm._21 = x;
        else
            mm.r[1] = DirectX::XMVectorSetX(mm.r[1], x);
    }
    constexpr inline void set_22(T y)
    {
        if (std::is_constant_evaluated())
            cm._22 = y;
        else
            mm.r[1] = DirectX::XMVectorSetY(mm.r[1], y);
    }
    constexpr inline void set_23(T z)
    {
        if (std::is_constant_evaluated())
            cm._23 = z;
        else
            mm.r[1] = DirectX::XMVectorSetZ(mm.r[1], z);
    }
    constexpr inline void set_24(T w)
    {
        if (std::is_constant_evaluated())
            cm._24 = w;
        else
            mm.r[1] = DirectX::XMVectorSetW(mm.r[1], w);
    }

    /* mm.r[2] */
    constexpr inline T get_31() const
    {
        if (std::is_constant_evaluated())
            return cm._31;
        else
            return DirectX::XMVectorGetX(mm.r[2]);
    }
    constexpr inline T get_32() const
    {
        if (std::is_constant_evaluated())
            return cm._32;
        else
            return DirectX::XMVectorGetY(mm.r[2]);
    }
    constexpr inline T get_33() const
    {
        if (std::is_constant_evaluated())
            return cm._33;
        else
            return DirectX::XMVectorGetZ(mm.r[2]);
    }
    constexpr inline T get_34() const
    {
        if (std::is_constant_evaluated())
            return cm._34;
        else
            return DirectX::XMVectorGetW(mm.r[2]);
    }
    constexpr inline void set_31(T x)
    {
        if (std::is_constant_evaluated())
            cm._31 = x;
        else
            mm.r[2] = DirectX::XMVectorSetX(mm.r[2], x);
    }
    constexpr inline void set_32(T y)
    {
        if (std::is_constant_evaluated())
            cm._32 = y;
        else
            mm.r[2] = DirectX::XMVectorSetY(mm.r[2], y);
    }
    constexpr inline void set_33(T z)
    {
        if (std::is_constant_evaluated())
            cm._33 = z;
        else
            mm.r[2] = DirectX::XMVectorSetZ(mm.r[2], z);
    }
    constexpr inline void set_34(T w)
    {
        if (std::is_constant_evaluated())
            cm._34 = w;
        else
            mm.r[2] = DirectX::XMVectorSetW(mm.r[2], w);
    }

    /* mm.r[3] */
    constexpr inline T get_41() const
    {
        if (std::is_constant_evaluated())
            return cm._41;
        else
            return DirectX::XMVectorGetX(mm.r[3]);
    }
    constexpr inline T get_42() const
    {
        if (std::is_constant_evaluated())
            return cm._42;
        else
            return DirectX::XMVectorGetY(mm.r[3]);
    }
    constexpr inline T get_43() const
    {
        if (std::is_constant_evaluated())
            return cm._43;
        else
            return DirectX::XMVectorGetZ(mm.r[3]);
    }
    constexpr inline T get_44() const
    {
        if (std::is_constant_evaluated())
            return cm._44;
        else
            return DirectX::XMVectorGetW(mm.r[3]);
    }
    constexpr inline void set_41(T x)
    {
        if (std::is_constant_evaluated())
            cm._41 = x;
        else
            mm.r[3] = DirectX::XMVectorSetX(mm.r[3], x);
    }
    constexpr inline void set_42(T y)
    {
        if (std::is_constant_evaluated())
            cm._42 = y;
        else
            mm.r[3] = DirectX::XMVectorSetY(mm.r[3], y);
    }
    constexpr inline void set_43(T z)
    {
        if (std::is_constant_evaluated())
            cm._43 = z;
        else
            mm.r[3] = DirectX::XMVectorSetZ(mm.r[3], z);
    }
    constexpr inline void set_44(T w)
    {
        if (std::is_constant_evaluated())
            cm._44 = w;
        else
            mm.r[3] = DirectX::XMVectorSetW(mm.r[3], w);
    }

    __declspec(property(get = get_11, put = set_11)) T _11;
    __declspec(property(get = get_12, put = set_12)) T _12;
    __declspec(property(get = get_13, put = set_13)) T _13;
    __declspec(property(get = get_14, put = set_14)) T _14;
    __declspec(property(get = get_21, put = set_21)) T _21;
    __declspec(property(get = get_22, put = set_22)) T _22;
    __declspec(property(get = get_23, put = set_23)) T _23;
    __declspec(property(get = get_24, put = set_24)) T _24;
    __declspec(property(get = get_31, put = set_31)) T _31;
    __declspec(property(get = get_32, put = set_32)) T _32;
    __declspec(property(get = get_33, put = set_33)) T _33;
    __declspec(property(get = get_34, put = set_34)) T _34;
    __declspec(property(get = get_41, put = set_41)) T _41;
    __declspec(property(get = get_42, put = set_42)) T _42;
    __declspec(property(get = get_43, put = set_43)) T _43;
    __declspec(property(get = get_44, put = set_44)) T _44;

    constexpr inline _matrix() = default;
    constexpr inline _matrix(T a0, T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8, T a9, T a10, T a11, T a12, T a13, T a14, T a15)
    {
        if (std::is_constant_evaluated())
            cm = DirectX::XMFLOAT4X4A(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15);
        else
            mm = DirectX::XMMatrixSet(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15);
    }

    // Class members
    constexpr ICF SelfRef set(const Self& a)
    {
        mm = a.mm;
        return *this;
    }
    constexpr ICF SelfRef set(const Tvector& R, const Tvector& N, const Tvector& D, const Tvector& C)
    {
        if (std::is_constant_evaluated())
        {
            vm[0].set(R, 0);
            vm[1].set(N, 0);
            vm[2].set(D, 0);
        }
        else
        {
            const DirectX::XMFLOAT3* r0 = reinterpret_cast<const DirectX::XMFLOAT3*>(&R);
            const DirectX::XMFLOAT3* r1 = reinterpret_cast<const DirectX::XMFLOAT3*>(&N);
            const DirectX::XMFLOAT3* r2 = reinterpret_cast<const DirectX::XMFLOAT3*>(&D);
            const DirectX::XMFLOAT3* r3 = reinterpret_cast<const DirectX::XMFLOAT3*>(&C);

            mm = DirectX::XMMATRIX(DirectX::XMLoadFloat3(r0), DirectX::XMLoadFloat3(r1), DirectX::XMLoadFloat3(r2), DirectX::XMLoadFloat3(r3));
        }

        vm[3].set(C, 1);

        return *this;
    }
    constexpr ICF SelfRef identity(void)
    {
        mm = DirectX::XMMatrixIdentity();
        return *this;
    }
    constexpr inline SelfRef rotation(const _quaternion<T>& Q);
    constexpr ICF SelfRef mk_xform(const _quaternion<T>& Q, const Tvector& V);

    // Multiply RES = A[4x4]*B[4x4] (WITH projection)
    constexpr ICF SelfRef mul(const Self& A, const Self& B)
    {
        VERIFY((this != &A) && (this != &B));

        if (std::is_constant_evaluated())
        {
            _11 = A._11 * B._11 + A._21 * B._12 + A._31 * B._13 + A._41 * B._14;
            _12 = A._12 * B._11 + A._22 * B._12 + A._32 * B._13 + A._42 * B._14;
            _13 = A._13 * B._11 + A._23 * B._12 + A._33 * B._13 + A._43 * B._14;
            _14 = A._14 * B._11 + A._24 * B._12 + A._34 * B._13 + A._44 * B._14;

            _21 = A._11 * B._21 + A._21 * B._22 + A._31 * B._23 + A._41 * B._24;
            _22 = A._12 * B._21 + A._22 * B._22 + A._32 * B._23 + A._42 * B._24;
            _23 = A._13 * B._21 + A._23 * B._22 + A._33 * B._23 + A._43 * B._24;
            _24 = A._14 * B._21 + A._24 * B._22 + A._34 * B._23 + A._44 * B._24;

            _31 = A._11 * B._31 + A._21 * B._32 + A._31 * B._33 + A._41 * B._34;
            _32 = A._12 * B._31 + A._22 * B._32 + A._32 * B._33 + A._42 * B._34;
            _33 = A._13 * B._31 + A._23 * B._32 + A._33 * B._33 + A._43 * B._34;
            _34 = A._14 * B._31 + A._24 * B._32 + A._34 * B._33 + A._44 * B._34;

            _41 = A._11 * B._41 + A._21 * B._42 + A._31 * B._43 + A._41 * B._44;
            _42 = A._12 * B._41 + A._22 * B._42 + A._32 * B._43 + A._42 * B._44;
            _43 = A._13 * B._41 + A._23 * B._42 + A._33 * B._43 + A._43 * B._44;
            _44 = A._14 * B._41 + A._24 * B._42 + A._34 * B._43 + A._44 * B._44;
        }
        else
            mm = DirectX::XMMatrixMultiply(B.mm, A.mm);

        return *this;
    }

    // Multiply RES = A[4x3]*B[4x3] (no projection), faster than ordinary multiply
    constexpr ICF SelfRef mul_43(const Self& A, const Self& B)
    {
        VERIFY((this != &A) && (this != &B));
        _11 = A._11 * B._11 + A._21 * B._12 + A._31 * B._13;
        _12 = A._12 * B._11 + A._22 * B._12 + A._32 * B._13;
        _13 = A._13 * B._11 + A._23 * B._12 + A._33 * B._13;
        _14 = 0;

        _21 = A._11 * B._21 + A._21 * B._22 + A._31 * B._23;
        _22 = A._12 * B._21 + A._22 * B._22 + A._32 * B._23;
        _23 = A._13 * B._21 + A._23 * B._22 + A._33 * B._23;
        _24 = 0;

        _31 = A._11 * B._31 + A._21 * B._32 + A._31 * B._33;
        _32 = A._12 * B._31 + A._22 * B._32 + A._32 * B._33;
        _33 = A._13 * B._31 + A._23 * B._32 + A._33 * B._33;
        _34 = 0;

        _41 = A._11 * B._41 + A._21 * B._42 + A._31 * B._43 + A._41;
        _42 = A._12 * B._41 + A._22 * B._42 + A._32 * B._43 + A._42;
        _43 = A._13 * B._41 + A._23 * B._42 + A._33 * B._43 + A._43;
        _44 = 1;
        return *this;
    }
    constexpr inline SelfRef mulA_44(const Self& A) // mul after
    {
        Self B;
        B.set(*this);
        mul(A, B);
        return *this;
    };
    constexpr inline SelfRef mulB_44(const Self& B) // mul before
    {
        Self A;
        A.set(*this);
        mul(A, B);
        return *this;
    };
    constexpr ICF SelfRef mulA_43(const Self& A) // mul after (no projection)
    {
        Self B;
        B.set(*this);
        mul_43(A, B);
        return *this;
    };
    constexpr ICF SelfRef mulB_43(const Self& B) // mul before (no projection)
    {
        Self A;
        A.set(*this);
        mul_43(A, B);
        return *this;
    };
    constexpr inline SelfRef invert(const Self& a)
    { // important: this is 4x3 invert, not the 4x4 one
        // faster than self-invert
        T fDetInv = (a._11 * (a._22 * a._33 - a._23 * a._32) - a._12 * (a._21 * a._33 - a._23 * a._31) + a._13 * (a._21 * a._32 - a._22 * a._31));

        VERIFY(_abs(fDetInv) > flt_zero);
        fDetInv = 1.0f / fDetInv;

        _11 = fDetInv * (a._22 * a._33 - a._23 * a._32);
        _12 = -fDetInv * (a._12 * a._33 - a._13 * a._32);
        _13 = fDetInv * (a._12 * a._23 - a._13 * a._22);
        _14 = 0.0f;

        _21 = -fDetInv * (a._21 * a._33 - a._23 * a._31);
        _22 = fDetInv * (a._11 * a._33 - a._13 * a._31);
        _23 = -fDetInv * (a._11 * a._23 - a._13 * a._21);
        _24 = 0.0f;

        _31 = fDetInv * (a._21 * a._32 - a._22 * a._31);
        _32 = -fDetInv * (a._11 * a._32 - a._12 * a._31);
        _33 = fDetInv * (a._11 * a._22 - a._12 * a._21);
        _34 = 0.0f;

        _41 = -(a._41 * _11 + a._42 * _21 + a._43 * _31);
        _42 = -(a._41 * _12 + a._42 * _22 + a._43 * _32);
        _43 = -(a._41 * _13 + a._42 * _23 + a._43 * _33);
        _44 = 1.0f;
        return *this;
    }

    constexpr inline bool invert_b(const Self& a)
    { // important: this is 4x3 invert, not the 4x4 one
        // faster than self-invert
        T fDetInv = (a._11 * (a._22 * a._33 - a._23 * a._32) - a._12 * (a._21 * a._33 - a._23 * a._31) + a._13 * (a._21 * a._32 - a._22 * a._31));

        if (_abs(fDetInv) <= flt_zero)
            return false;
        fDetInv = 1.0f / fDetInv;

        _11 = fDetInv * (a._22 * a._33 - a._23 * a._32);
        _12 = -fDetInv * (a._12 * a._33 - a._13 * a._32);
        _13 = fDetInv * (a._12 * a._23 - a._13 * a._22);
        _14 = 0.0f;

        _21 = -fDetInv * (a._21 * a._33 - a._23 * a._31);
        _22 = fDetInv * (a._11 * a._33 - a._13 * a._31);
        _23 = -fDetInv * (a._11 * a._23 - a._13 * a._21);
        _24 = 0.0f;

        _31 = fDetInv * (a._21 * a._32 - a._22 * a._31);
        _32 = -fDetInv * (a._11 * a._32 - a._12 * a._31);
        _33 = fDetInv * (a._11 * a._22 - a._12 * a._21);
        _34 = 0.0f;

        _41 = -(a._41 * _11 + a._42 * _21 + a._43 * _31);
        _42 = -(a._41 * _12 + a._42 * _22 + a._43 * _32);
        _43 = -(a._41 * _13 + a._42 * _23 + a._43 * _33);
        _44 = 1.0f;
        return true;
    }

    constexpr inline SelfRef invert() // slower than invert other matrix
    {
        Self a;
        a.set(*this);
        invert(a);

        return *this;
    }

    constexpr inline SelfRef invert_44(const Self& a)
    {
        if (std::is_constant_evaluated())
        {
            const T &a11 = a.cm._11, &a12 = a.cm._12, &a13 = a.cm._13, &a14 = a.cm._14;
            const T &a21 = a.cm._21, &a22 = a.cm._22, &a23 = a.cm._23, &a24 = a.cm._24;
            const T &a31 = a.cm._31, &a32 = a.cm._32, &a33 = a.cm._33, &a34 = a.cm._34;
            const T &a41 = a.cm._41, &a42 = a.cm._42, &a43 = a.cm._43, &a44 = a.cm._44;

            T mn1 = a33 * a44 - a34 * a43;
            T mn2 = a32 * a44 - a34 * a42;
            T mn3 = a32 * a43 - a33 * a42;
            T mn4 = a31 * a44 - a34 * a41;
            T mn5 = a31 * a43 - a33 * a41;
            T mn6 = a31 * a42 - a32 * a41;

            T A11 = a22 * mn1 - a23 * mn2 + a24 * mn3;
            T A12 = -(a21 * mn1 - a23 * mn4 + a24 * mn5);
            T A13 = a21 * mn2 - a22 * mn4 + a24 * mn6;
            T A14 = -(a21 * mn3 - a22 * mn5 + a23 * mn6);

            T detInv = a11 * A11 + a12 * A12 + a13 * A13 + a14 * A14;
            VERIFY(_abs(detInv) > flt_zero);

            detInv = 1.f / detInv;

            _11 = detInv * A11;
            _12 = -detInv * (a12 * mn1 - a32 * (a13 * a44 - a43 * a14) + a42 * (a13 * a34 - a33 * a14));
            _13 = detInv * (a12 * (a23 * a44 - a43 * a24) - a22 * (a13 * a44 - a43 * a14) + a42 * (a13 * a24 - a23 * a14));
            _14 = -detInv * (a12 * (a23 * a34 - a33 * a24) - a22 * (a13 * a34 - a33 * a14) + a32 * (a13 * a24 - a23 * a14));

            _21 = detInv * A12;
            _22 = detInv * (a11 * mn1 - a31 * (a13 * a44 - a43 * a14) + a41 * (a13 * a34 - a33 * a14));
            _23 = -detInv * (a11 * (a23 * a44 - a43 * a24) - a21 * (a13 * a44 - a43 * a14) + a41 * (a13 * a24 - a23 * a14));
            _24 = detInv * (a11 * (a23 * a34 - a33 * a24) - a21 * (a13 * a34 - a33 * a14) + a31 * (a13 * a24 - a23 * a14));

            _31 = detInv * A13;
            _32 = -detInv * (a11 * (a32 * a44 - a42 * a34) - a31 * (a12 * a44 - a42 * a14) + a41 * (a12 * a34 - a32 * a14));
            _33 = detInv * (a11 * (a22 * a44 - a42 * a24) - a21 * (a12 * a44 - a42 * a14) + a41 * (a12 * a24 - a22 * a14));
            _34 = -detInv * (a11 * (a22 * a34 - a32 * a24) - a21 * (a12 * a34 - a32 * a14) + a31 * (a12 * a24 - a22 * a14));

            /*
                _11, _12, _13, _14;
                _21, _22, _23, _24;
                _31, _32, _33, _34;
                _41, _42, _43, _44;
            */

            _41 = detInv * A14;
            _42 = detInv * (a11 * (a32 * a43 - a42 * a33) - a31 * (a12 * a43 - a42 * a13) + a41 * (a12 * a33 - a32 * a13));
            _43 = -detInv * (a11 * (a22 * a43 - a42 * a23) - a21 * (a12 * a43 - a42 * a13) + a41 * (a12 * a23 - a22 * a13));
            _44 = detInv * (a11 * (a22 * a33 - a32 * a23) - a21 * (a12 * a33 - a32 * a13) + a31 * (a12 * a23 - a22 * a13));
        }
        else
            mm = DirectX::XMMatrixInverse(nullptr, a.mm);

        return *this;
    }

    constexpr inline SelfRef transpose(const Self& matSource) // faster version of transpose
    {
        if (std::is_constant_evaluated())
        {
            _11 = matSource._11;
            _12 = matSource._21;
            _13 = matSource._31;
            _14 = matSource._41;
            _21 = matSource._12;
            _22 = matSource._22;
            _23 = matSource._32;
            _24 = matSource._42;
            _31 = matSource._13;
            _32 = matSource._23;
            _33 = matSource._33;
            _34 = matSource._43;
            _41 = matSource._14;
            _42 = matSource._24;
            _43 = matSource._34;
            _44 = matSource._44;
        }
        else
            mm = DirectX::XMMatrixTranspose(matSource.mm);

        return *this;
    }
    constexpr inline SelfRef transpose() // self transpose - slower
    {
        if (std::is_constant_evaluated())
        {
            Self a;
            a.set(*this);
            transpose(a);
        }
        else
            mm = DirectX::XMMatrixTranspose(mm);

        return *this;
    }
    constexpr inline SelfRef translate(const Tvector& Loc) // setup translation matrix
    {
        mm.r[0] = DirectX::g_XMIdentityR0.v;
        mm.r[1] = DirectX::g_XMIdentityR1.v;
        mm.r[2] = DirectX::g_XMIdentityR2.v;
        vm[3].set(Loc, 1);

        return *this;
    }
    constexpr inline SelfRef translate(T _x, T _y, T _z) // setup translation matrix
    {
        mm.r[0] = DirectX::g_XMIdentityR0.v;
        mm.r[1] = DirectX::g_XMIdentityR1.v;
        mm.r[2] = DirectX::g_XMIdentityR2.v;
        vm[3].set(_x, _y, _z, 1);

        return *this;
    }
    constexpr inline SelfRef translate_over(const Tvector& Loc) // modify only translation
    {
        vm[3].set(Loc, vm[3].w);
        return *this;
    }
    constexpr inline SelfRef translate_over(T _x, T _y, T _z) // modify only translation
    {
        vm[3].set(_x, _y, _z, vm[3].w);
        return *this;
    }
    constexpr inline SelfRef translate_add(const Tvector& Loc) // combine translation
    {
        DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&Loc));

        mm.r[3] = DirectX::XMVectorAdd(mm.r[3], vec);

        return *this;
    }
    constexpr inline SelfRef scale(T x, T y, T z) // setup scale matrix
    {
        mm = DirectX::XMMatrixScaling(x, y, z);
        return *this;
    }
    constexpr inline SelfRef scale(const Tvector& v) // setup scale matrix
    {
        return scale(v.x, v.y, v.z);
    }

    constexpr inline SelfRef rotateX(T Angle) // rotation about X axis
    {
        if (std::is_constant_evaluated())
        {
            T cosa = _cos(Angle);
            T sina = _sin(Angle);

            mm.r[0] = DirectX::g_XMIdentityR0.v;
            vm[1].set(0, cosa, sina, 0);
            vm[2].set(0, -sina, cosa, 0);
            mm.r[3] = DirectX::g_XMIdentityR3.v;
        }
        else
            mm = DirectX::XMMatrixRotationX(Angle);

        return *this;
    }
    constexpr inline SelfRef rotateY(T Angle) // rotation about Y axis
    {
        if (std::is_constant_evaluated())
        {
            T cosa = _cos(Angle);
            T sina = _sin(Angle);

            vm[0].set(cosa, 0, -sina, 0);
            mm.r[1] = DirectX::g_XMIdentityR1.v;
            vm[2].set(sina, 0, cosa, 0);
            mm.r[3] = DirectX::g_XMIdentityR3.v;
        }
        else
            mm = DirectX::XMMatrixRotationY(Angle);

        return *this;
    }
    constexpr inline SelfRef rotateZ(T Angle) // rotation about Z axis
    {
        if (std::is_constant_evaluated())
        {
            T cosa = _cos(Angle);
            T sina = _sin(Angle);

            vm[0].set(cosa, sina, 0, 0);
            vm[1].set(-sina, cosa, 0, 0);
            mm.r[2] = DirectX::g_XMIdentityR2.v;
            mm.r[3] = DirectX::g_XMIdentityR3.v;
        }
        else
            mm = DirectX::XMMatrixRotationZ(Angle);

        return *this;
    }

    constexpr inline SelfRef rotation(const Tvector& vdir, const Tvector& vnorm)
    {
        Tvector vright;
        vright.crossproduct(vnorm, vdir).normalize();

        vm[0].set(vright.x, vright.y, vright.z, 0);
        vm[1].set(vnorm.x, vnorm.y, vnorm.z, 0);
        vm[2].set(vdir.x, vdir.y, vdir.z, 0);
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }

    constexpr inline SelfRef mapXYZ()
    {
        identity();
        return *this;
    }
    constexpr inline SelfRef mapXZY()
    {
        mm.r[0] = DirectX::g_XMIdentityR0.v;
        mm.r[1] = DirectX::g_XMIdentityR2.v;
        mm.r[2] = DirectX::g_XMIdentityR1.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef mapYXZ()
    {
        mm.r[0] = DirectX::g_XMIdentityR1.v;
        mm.r[1] = DirectX::g_XMIdentityR0.v;
        mm.r[2] = DirectX::g_XMIdentityR2.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef mapYZX()
    {
        mm.r[0] = DirectX::g_XMIdentityR1.v;
        mm.r[1] = DirectX::g_XMIdentityR2.v;
        mm.r[2] = DirectX::g_XMIdentityR0.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef mapZXY()
    {
        mm.r[0] = DirectX::g_XMIdentityR2.v;
        mm.r[1] = DirectX::g_XMIdentityR0.v;
        mm.r[2] = DirectX::g_XMIdentityR1.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef mapZYX()
    {
        mm.r[0] = DirectX::g_XMIdentityR2.v;
        mm.r[1] = DirectX::g_XMIdentityR1.v;
        mm.r[2] = DirectX::g_XMIdentityR0.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }

    constexpr inline SelfRef rotation(const Tvector& axis, T Angle)
    {
        T Sine, Cosine;

        DirectX::XMScalarSinCos(&Sine, &Cosine, Angle);

        vm[0].set(axis.x * axis.x + (1 - axis.x * axis.x) * Cosine, axis.x * axis.y * (1 - Cosine) + axis.z * Sine, axis.x * axis.z * (1 - Cosine) - axis.y * Sine, 0);
        vm[1].set(axis.x * axis.y * (1 - Cosine) - axis.z * Sine, axis.y * axis.y + (1 - axis.y * axis.y) * Cosine, axis.y * axis.z * (1 - Cosine) + axis.x * Sine, 0);
        vm[2].set(axis.x * axis.z * (1 - Cosine) + axis.y * Sine, axis.y * axis.z * (1 - Cosine) - axis.x * Sine, axis.z * axis.z + (1 - axis.z * axis.z) * Cosine, 0);
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }

    // mirror X
    constexpr inline SelfRef mirrorX()
    {
        mm.r[0] = DirectX::g_XMNegIdentityR0.v;
        mm.r[1] = DirectX::g_XMIdentityR1.v;
        mm.r[2] = DirectX::g_XMIdentityR2.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef mirrorX_over()
    {
        vm[0].x = -1;
        return *this;
    }
    constexpr inline SelfRef mirrorX_add()
    {
        vm[0].x *= -1;
        return *this;
    }

    // mirror Y
    constexpr inline SelfRef mirrorY()
    {
        mm.r[0] = DirectX::g_XMIdentityR0.v;
        mm.r[1] = DirectX::g_XMNegIdentityR1.v;
        mm.r[2] = DirectX::g_XMIdentityR2.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef mirrorY_over()
    {
        vm[1].y = -1;
        return *this;
    }
    constexpr inline SelfRef mirrorY_add()
    {
        vm[1].y *= -1;
        return *this;
    }

    // mirror Z
    constexpr inline SelfRef mirrorZ()
    {
        mm.r[0] = DirectX::g_XMIdentityR0.v;
        mm.r[1] = DirectX::g_XMIdentityR1.v;
        mm.r[2] = DirectX::g_XMNegIdentityR2.v;
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef mirrorZ_over()
    {
        vm[2].z = -1;
        return *this;
    }
    constexpr inline SelfRef mirrorZ_add()
    {
        vm[2].z *= -1;
        return *this;
    }
    constexpr inline SelfRef mul(const Self& A, T v)
    {
        vm[0].mul(A.vm[0], v);
        vm[1].mul(A.vm[1], v);
        vm[2].mul(A.vm[2], v);
        vm[3].mul(A.vm[3], v);

        return *this;
    }
    constexpr inline SelfRef mul(T v)
    {
        vm[0].mul(v);
        vm[1].mul(v);
        vm[2].mul(v);
        vm[3].mul(v);

        return *this;
    }
    constexpr inline SelfRef div(const Self& A, T v)
    {
        VERIFY(_abs(v) > 0.000001f);

        vm[0].div(A.vm[0], v);
        vm[1].div(A.vm[1], v);
        vm[2].div(A.vm[2], v);
        vm[3].div(A.vm[3], v);

        return *this;
    }
    constexpr inline SelfRef div(T v)
    {
        VERIFY(_abs(v) > 0.000001f);

        vm[0].div(v);
        vm[1].div(v);
        vm[2].div(v);
        vm[3].div(v);

        return *this;
    }
    // fov
    constexpr inline SelfRef build_projection(T fFOV, T fAspect, T fNearPlane, T fFarPlane) { return build_projection_HAT(tanf(fFOV / 2.f), fAspect, fNearPlane, fFarPlane); }
    // half_fov-angle-tangent
    constexpr inline SelfRef build_projection_HAT(T HAT, T fAspect, T fNearPlane, T fFarPlane)
    {
        VERIFY(_abs(fFarPlane - fNearPlane) > EPS_S);
        VERIFY(_abs(HAT) > EPS_S);

        T cot = T(1) / HAT;
        T w = fAspect * cot;
        T h = T(1) * cot;
        T Q = fFarPlane / (fFarPlane - fNearPlane);

        vm[0].set(w, 0, 0, 0);
        vm[1].set(0, h, 0, 0);
        vm[2].set(0, 0, Q, 1);
        vm[3].set(0, 0, -Q * fNearPlane, 0);

        return *this;
    }
    constexpr inline SelfRef build_projection_ortho(T w, T h, T zn, T zf)
    {
        vm[0].set(T(2) / w, 0, 0, 0);
        vm[1].set(0, T(2) / h, 0, 0);
        vm[2].set(0, 0, T(1) / (zf - zn), 0);
        vm[3].set(0, 0, zn / (zn - zf), T(1));

        return *this;
    }
    constexpr inline SelfRef build_camera(const Tvector& vFrom, const Tvector& vAt, const Tvector& vWorldUp)
    {
        // Get the z basis vector3, which points straight ahead. This is the
        // difference from the eyepoint to the lookat point.
        Tvector vView;
        vView.sub(vAt, vFrom).normalize();

        // Get the dot product, and calculate the projection of the z basis
        // vector3 onto the up vector3. The projection is the y basis vector3.
        T fDotProduct = vWorldUp.dotproduct(vView);

        Tvector vUp;
        vUp.mul(vView, -fDotProduct).add(vWorldUp).normalize();

        // The x basis vector3 is found simply with the cross product of the y
        // and z basis vectors
        Tvector vRight;
        vRight.crossproduct(vUp, vView);

        // Start building the Device.mView. The first three rows contains the basis
        // vectors used to rotate the view to point at the lookat point
        vm[0].set(vRight.x, vUp.x, vView.x, 0.0f);
        vm[1].set(vRight.y, vUp.y, vView.y, 0.0f);
        vm[2].set(vRight.z, vUp.z, vView.z, 0.0f);

        // Do the translation values (rotations are still about the eyepoint)
        vm[3].set(-vFrom.dotproduct(vRight), -vFrom.dotproduct(vUp), -vFrom.dotproduct(vView), 1.0f);

        return *this;
    }
    constexpr inline SelfRef build_camera_dir(const Tvector& vFrom, const Tvector& vView, const Tvector& vWorldUp)
    {
        // Get the dot product, and calculate the projection of the z basis
        // vector3 onto the up vector3. The projection is the y basis vector3.
        T fDotProduct = vWorldUp.dotproduct(vView);

        Tvector vUp;
        vUp.mul(vView, -fDotProduct).add(vWorldUp).normalize();

        // The x basis vector3 is found simply with the cross product of the y
        // and z basis vectors
        Tvector vRight;
        vRight.crossproduct(vUp, vView);

        // Start building the Device.mView. The first three rows contains the basis
        // vectors used to rotate the view to point at the lookat point
        vm[0].set(vRight.x, vUp.x, vView.x, 0.0f);
        vm[1].set(vRight.y, vUp.y, vView.y, 0.0f);
        vm[2].set(vRight.z, vUp.z, vView.z, 0.0f);

        // Do the translation values (rotations are still about the eyepoint)
        vm[3].set(-vFrom.dotproduct(vRight), -vFrom.dotproduct(vUp), -vFrom.dotproduct(vView), 1.0f);

        return *this;
    }

    constexpr inline SelfRef inertion(const Self& mat, T v)
    {
        vm[0].lerp(mat.vm[0], vm[0], v);
        vm[1].lerp(mat.vm[1], vm[1], v);
        vm[2].lerp(mat.vm[2], vm[2], v);
        vm[3].lerp(mat.vm[3], vm[3], v);

        return *this;
    }
    constexpr ICF void transform_tiny(Tvector& dest, const Tvector& v) const // preferred to use
    {
        dest.x = v.x * _11 + v.y * _21 + v.z * _31 + _41;
        dest.y = v.x * _12 + v.y * _22 + v.z * _32 + _42;
        dest.z = v.x * _13 + v.y * _23 + v.z * _33 + _43;
    }
    constexpr ICF void transform_tiny32(Fvector2& dest, const Tvector& v) const // preferred to use
    {
        dest.x = v.x * _11 + v.y * _21 + v.z * _31 + _41;
        dest.y = v.x * _12 + v.y * _22 + v.z * _32 + _42;
    }
    constexpr ICF void transform_tiny23(Tvector& dest, const Fvector2& v) const // preferred to use
    {
        dest.x = v.x * _11 + v.y * _21 + _41;
        dest.y = v.x * _12 + v.y * _22 + _42;
        dest.z = v.x * _13 + v.y * _23 + _43;
    }
    constexpr ICF void transform_dir(Tvector& dest, const Tvector& v) const // preferred to use
    {
        dest.x = v.x * _11 + v.y * _21 + v.z * _31;
        dest.y = v.x * _12 + v.y * _22 + v.z * _32;
        dest.z = v.x * _13 + v.y * _23 + v.z * _33;
    }
    constexpr inline void transform(Fvector4& dest, const Tvector& v) const // preferred to use
    {
        dest.w = v.x * _14 + v.y * _24 + v.z * _34 + _44;
        dest.x = (v.x * _11 + v.y * _21 + v.z * _31 + _41) / dest.w;
        dest.y = (v.x * _12 + v.y * _22 + v.z * _32 + _42) / dest.w;
        dest.z = (v.x * _13 + v.y * _23 + v.z * _33 + _43) / dest.w;
    }
    constexpr inline void transform(Tvector& dest, const Tvector& v) const // preferred to use
    {
        T iw = 1.f / (v.x * _14 + v.y * _24 + v.z * _34 + _44);
        dest.x = (v.x * _11 + v.y * _21 + v.z * _31 + _41) * iw;
        dest.y = (v.x * _12 + v.y * _22 + v.z * _32 + _42) * iw;
        dest.z = (v.x * _13 + v.y * _23 + v.z * _33 + _43) * iw;
    }

    constexpr inline void transform(Fvector4& dest, const Fvector4& v) const // preferred to use
    {
        dest.w = v.x * _14 + v.y * _24 + v.z * _34 + v.w * _44;
        dest.x = v.x * _11 + v.y * _21 + v.z * _31 + v.w * _41;
        dest.y = v.x * _12 + v.y * _22 + v.z * _32 + v.w * _42;
        dest.z = v.x * _13 + v.y * _23 + v.z * _33 + v.w * _43;
    }

    constexpr ICF void transform_tiny(Tvector& v) const
    {
        Tvector res;
        transform_tiny(res, v);
        v.set(res);
    }
    constexpr inline void transform(Tvector& v) const
    {
        Tvector res;
        transform(res, v);
        v.set(res);
    }
    constexpr ICF void transform_dir(Tvector& v) const
    {
        Tvector res;
        transform_dir(res, v);
        v.set(res);
    }
    constexpr ICF SelfRef setHPB(T h, T p, T b)
    {
        T _ch, _cp, _cb, _sh, _sp, _sb, _cc, _cs, _sc, _ss;

        DirectX::XMScalarSinCos(&_sh, &_ch, h);
        DirectX::XMScalarSinCos(&_sp, &_cp, p);
        DirectX::XMScalarSinCos(&_sb, &_cb, b);

        _cc = _ch * _cb;
        _cs = _ch * _sb;
        _sc = _sh * _cb;
        _ss = _sh * _sb;

        vm[0].set(_cc - _sp * _ss, -_cp * _sb, _sp * _cs + _sc, 0);
        vm[1].set(_sp * _sc + _cs, _cp * _cb, _ss - _sp * _cc, 0);
        vm[2].set(-_cp * _sh, _sp, _cp * _ch, 0);
        mm.r[3] = DirectX::g_XMIdentityR3.v;

        return *this;
    }
    constexpr inline SelfRef setXYZ(T x, T y, T z) { return setHPB(y, x, z); }
    constexpr inline SelfRef setXYZ(Tvector const& xyz) { return setHPB(xyz.y, xyz.x, xyz.z); }
    constexpr inline SelfRef setXYZi(T x, T y, T z) { return setHPB(-y, -x, -z); }
    constexpr inline SelfRef setXYZi(Tvector const& xyz) { return setHPB(-xyz.y, -xyz.x, -xyz.z); }
    //
    constexpr inline void getHPB(T& h, T& p, T& b) const
    {
        T cy = _sqrt(vm[1].y * vm[1].y + vm[0].y * vm[0].y);
        if (cy > 16.0f * type_epsilon(T))
        {
            h = (T)-atan2(vm[2].x, vm[2].z);
            p = (T)-atan2(-vm[2].y, cy);
            b = (T)-atan2(vm[0].y, vm[1].y);
        }
        else
        {
            h = (T)-atan2(-vm[0].z, vm[0].x);
            p = (T)-atan2(-vm[2].y, cy);
            b = 0;
        }
    }
    constexpr inline void getHPB(Tvector& hpb) const { getHPB(hpb.x, hpb.y, hpb.z); }
    constexpr inline void getXYZ(T& x, T& y, T& z) const { getHPB(y, x, z); }
    constexpr inline void getXYZ(Tvector& xyz) const { getXYZ(xyz.x, xyz.y, xyz.z); }
    constexpr inline void getXYZi(T& x, T& y, T& z) const
    {
        getHPB(y, x, z);
        x *= -1.f;
        y *= -1.f;
        z *= -1.f;
    }
    constexpr inline void getXYZi(Tvector& xyz) const
    {
        getXYZ(xyz.x, xyz.y, xyz.z);
        xyz.mul(-1.f);
    }
};

typedef _matrix<float> Fmatrix;
static_assert(sizeof(Fmatrix) == 64);

template <class T>
BOOL _valid(const _matrix<T>& m)
{
    return _valid(m.i) && _valid(m._14) && _valid(m.j) && _valid(m._24) && _valid(m.k) && _valid(m._34) && _valid(m.c) && _valid(m._44);
}

extern XRCORE_API Fmatrix Fidentity;

#endif /* __XR_CORE_MATRIX_H */
