#include "stdafx.h"

#include "torridZone.h"

#include "../xr_3da/objectanimator.h"
#include "xrServer_Objects_ALife_Monsters.h"

CTorridZone::CTorridZone() { m_animator = xr_new<CObjectAnimator>(); }
CTorridZone::~CTorridZone() { xr_delete(m_animator); }

tmc::task<bool> CTorridZone::net_Spawn(CSE_Abstract* DC)
{
    if (!co_await inherited::net_Spawn(DC))
        co_return false;

    CSE_ALifeTorridZone* zone = smart_cast<CSE_ALifeTorridZone*>(DC);
    VERIFY(zone);

    m_animator->Load(zone->get_motion());
    m_animator->Play(true);

    m_b_always_fastmode = true;

    co_return true;
}

void CTorridZone::UpdateWorkload(u32 dt)
{
    inherited::UpdateWorkload(dt);
    m_animator->Update(float(dt) / 1000.f);
    XFORM().set(m_animator->XFORM());
    OnMove();
}

tmc::task<void> CTorridZone::shedule_Update(u32 dt)
{
    co_await inherited::shedule_Update(dt);

    if (m_idle_sound._feedback())
        m_idle_sound.set_position(XFORM().c);
    if (m_blowout_sound._feedback())
        m_blowout_sound.set_position(XFORM().c);
    if (m_hit_sound._feedback())
        m_hit_sound.set_position(XFORM().c);
    if (m_entrance_sound._feedback())
        m_entrance_sound.set_position(XFORM().c);
}

void CTorridZone::GoEnabledState()
{
    inherited::GoEnabledState();
    m_animator->Stop();
    m_animator->Play(true);
}

void CTorridZone::GoDisabledState() { inherited::GoDisabledState(); }
