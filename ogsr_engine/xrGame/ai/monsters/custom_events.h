#pragma once
#include "monster_event_manager_defs.h"

//////////////////////////////////////////////////////////////////////////

struct CEventTAPrepareAnimation final : public IEventData
{
    u32 m_current_state;

    IC CEventTAPrepareAnimation(u32 state) : m_current_state(state) {}
};

//////////////////////////////////////////////////////////////////////////

struct CEventVelocityBounce final : public IEventData
{
    float m_ratio;

    IC CEventVelocityBounce(float ratio) : m_ratio(ratio) {}
};

//////////////////////////////////////////////////////////////////////////
