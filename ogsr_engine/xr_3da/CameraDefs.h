#pragma once

struct SBaseEffector : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(SBaseEffector);

public:
    CallMe::Delegate<void()> m_on_b_remove_callback;
    virtual ~SBaseEffector() = default;
};

struct SCamEffectorInfo
{
    Fvector p;
    Fvector d;
    Fvector n;
    Fvector r;
    float fFov;
    float fNear;
    float fFar;
    float fAspect;
    float offsetX; // Required for Nvidia Ansel
    float offsetY; // Required for Nvidia Ansel
    bool dont_apply;
    bool affected_on_hud;

    SCamEffectorInfo();
    SCamEffectorInfo(const SCamEffectorInfo&);
    SCamEffectorInfo(SCamEffectorInfo&&);

    SCamEffectorInfo& operator=(const SCamEffectorInfo&);
    SCamEffectorInfo& operator=(SCamEffectorInfo&&);
};

enum ECameraStyle : u32
{
    csCamDebug,
    csFirstEye,
    csLookAt,
    csMax,
    csFixed,
};

enum ECamEffectorType
{
    cefDemo = 0,
    cefNext
};

enum EEffectorPPType
{
    ppeNext = 0,
};

// refs
class CCameraBase;
class CEffectorCam;
class CEffectorPP;
