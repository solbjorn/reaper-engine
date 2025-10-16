// CameraBase.h: interface for the CCameraBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraDefs.h"

// refs
class CObject;

class CCameraBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CCameraBase);

protected:
    CObject* parent;

public:
    bool bClampYaw{};
    bool bClampPitch{};
    bool bClampRoll{};

    float yaw{};
    float pitch{};
    float roll{};

    enum
    {
        flRelativeLink = (1 << 0),
        flPositionRigid = (1 << 1),
        flDirectionRigid = (1 << 2),
    };

    Flags32 m_Flags;
    ECameraStyle style;

    Fvector2 lim_yaw{};
    Fvector2 lim_pitch{};
    Fvector2 lim_roll{};

    Fvector rot_speed;

    Fvector vPosition{};
    Fvector vDirection{0.0f, 0.0f, 1.0f};
    Fvector vNormal{0.0f, 1.0f, 0.0f};

    float f_fov{90.0f};
    float f_aspect{1.0f};
    int tag{};

public:
    CCameraBase(CObject* p, u32 flags);
    virtual ~CCameraBase();

    virtual void Load(LPCSTR section);

    void SetParent(CObject* p)
    {
        VERIFY(p);
        parent = p;
    }

    virtual void OnActivate(CCameraBase*) {}
    virtual void OnDeactivate() {}
    virtual void Move(int, float = 0.0f, float = 1.0f) {}
    virtual void Update(Fvector&, Fvector&) {}

    virtual void Get(Fvector& P, Fvector& D, Fvector& N) const
    {
        P.set(vPosition);
        D.set(vDirection);
        N.set(vNormal);
    }

    virtual void Set(const Fvector& P, const Fvector& D, const Fvector& N)
    {
        vPosition.set(P);
        vDirection.set(D);
        vNormal.set(N);
    }

    virtual void Set(float Y, float P, float R)
    {
        yaw = Y;
        pitch = P;
        roll = R;
    }

    virtual float GetWorldYaw() const { return 0.0f; }
    virtual float GetWorldPitch() const { return 0.0f; }
};
