#ifndef PHDISABLING_H
#define PHDISABLING_H

#include "DisablingParams.h"
#include "ode_include.h"

struct SDisableVector
{
    Fvector sum;
    Fvector previous;
    float UpdatePrevious(const Fvector& new_vector);
    float Update(const Fvector& new_vector);
    void Reset();
    void Init();
    float SumMagnitude();

    SDisableVector();
};

struct SDisableUpdateState
{
    bool disable;
    bool enable;
    void Reset();
    SDisableUpdateState& operator&=(SDisableUpdateState& ltate);
    SDisableUpdateState();
};

struct XR_NOVTABLE CBaseDisableData : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CBaseDisableData);

public:
    CBaseDisableData();
    ~CBaseDisableData() override = 0;

protected:
    u16 m_count;
    u16 m_frames;
    SDisableUpdateState m_stateL1;
    SDisableUpdateState m_stateL2;
    bool m_disabled;

protected:
    IC void CheckState(const SDisableUpdateState& state)
    {
        if (m_disabled)
            m_disabled = !state.enable;
        else
            m_disabled = state.disable;
    }

    void Disabling();
    void Reinit();
    virtual void Disable() = 0;
    virtual void ReEnable() = 0;
    virtual void UpdateL1() = 0;
    virtual void UpdateL2() = 0;
    virtual dBodyID get_body() = 0;
};

inline CBaseDisableData::~CBaseDisableData() = default;

class CPHDisablingBase : public virtual CBaseDisableData
{
    RTTI_DECLARE_TYPEINFO(CPHDisablingBase, CBaseDisableData);

public:
    ~CPHDisablingBase() override = default;

    void UpdateValues(const Fvector& new_pos, const Fvector& new_vel);
    virtual void UpdateL2();
    virtual void set_DisableParams(const SOneDDOParams& params);

protected:
    void Reinit();
    IC void CheckState(SDisableUpdateState& state, float vel, float accel)
    {
        if (vel < m_params.velocity && accel < m_params.acceleration)
            state.disable = true;
        if (vel > m_params.velocity * worldDisablingParams.reanable_factor || accel > m_params.acceleration * worldDisablingParams.reanable_factor)
            state.enable = true;
    }

protected:
    SDisableVector m_mean_velocity;
    SDisableVector m_mean_acceleration;
    SOneDDOParams m_params;
};

class CPHDisablingRotational : public CPHDisablingBase
{
    RTTI_DECLARE_TYPEINFO(CPHDisablingRotational, CPHDisablingBase);

public:
    CPHDisablingRotational();
    ~CPHDisablingRotational() override = default;

    void Reinit();
    virtual void UpdateL1();
    virtual void set_DisableParams(const SAllDDOParams& params);
};

class CPHDisablingTranslational : public CPHDisablingBase
{
    RTTI_DECLARE_TYPEINFO(CPHDisablingTranslational, CPHDisablingBase);

public:
    CPHDisablingTranslational();
    ~CPHDisablingTranslational() override = default;

    void Reinit();
    virtual void UpdateL1();
    virtual void set_DisableParams(const SAllDDOParams& params);
};

class CPHDisablingFull : public CPHDisablingTranslational, public CPHDisablingRotational
{
    RTTI_DECLARE_TYPEINFO(CPHDisablingFull, CPHDisablingTranslational, CPHDisablingRotational);

public:
    ~CPHDisablingFull() override = default;

    void Reinit();
    virtual void UpdateL1();
    virtual void UpdateL2();
    virtual void set_DisableParams(const SAllDDOParams& params);
};

#endif
