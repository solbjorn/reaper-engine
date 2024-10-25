#pragma once

struct SBaseEffector : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(SBaseEffector);

public:
    CallMe::Delegate<void()> m_on_b_remove_callback;

    ~SBaseEffector() override = default;
};

struct SCamEffectorInfo
{
    Fvector p{};
    Fvector d{0.0f, 0.0f, 1.0f};
    Fvector n{0.0f, 1.0f, 0.0f};
    Fvector r;

    float fFov{90.0f};
    float fNear{VIEWPORT_NEAR};
    float fFar{100.0f};
    float fAspect{1.0f};
    float offsetX{}; // Required for Nvidia Ansel
    float offsetY{}; // Required for Nvidia Ansel

    bool dont_apply{};
    bool affected_on_hud{true};

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

enum ECamEffectorType : u32
{
    cefDemo = 0,
    cefNext,
};

enum EEffectorPPType : u32
{
    ppeNext = 0,
};

// refs
class CCameraBase;
class CEffectorCam;
class CEffectorPP;
