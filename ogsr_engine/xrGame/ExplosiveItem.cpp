//////////////////////////////////////////////////////////////////////
// ExplosiveItem.cpp:	класс для вещи которая взрывается под
//						действием различных хитов (канистры,
//						балоны с газом и т.д.)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ExplosiveItem.h"

CExplosiveItem::CExplosiveItem() = default;
CExplosiveItem::~CExplosiveItem() = default;

void CExplosiveItem::Load(LPCSTR section)
{
    inherited::Load(section);
    CExplosive::Load(section);
    m_flags.set(FUsingCondition, TRUE);
    CDelayedActionFuse::Initialize(pSettings->r_float(section, "time_to_explode"), pSettings->r_float(section, "condition_to_explode"));
    VERIFY(pSettings->line_exist(section, "set_timer_particles"));
}

tmc::task<void> CExplosiveItem::net_Destroy()
{
    co_await inherited::net_Destroy();
    co_await CExplosive::net_Destroy();
}

void CExplosiveItem::Hit(SHit* pHDS)
{
    //	inherited::Hit(P,dir,who,element,position_in_object_space,impulse,hit_type);
    if (CDelayedActionFuse::isActive())
        pHDS->power = 0.f;
    inherited::Hit(pHDS);
    if (!CDelayedActionFuse::isActive() && CDelayedActionFuse::CheckCondition(GetCondition()) && CExplosive::Initiator() == u16(-1))
    {
        // запомнить того, кто взорвал вещь
        SetInitiator(pHDS->who->ID());
    }
}

void CExplosiveItem::StartTimerEffects() { CParticlesPlayer::StartParticles(pSettings->r_string<shared_str>(cNameSect(), "set_timer_particles"), Fvector().set(0, 1, 0), ID()); }

tmc::task<void> CExplosiveItem::OnEvent(NET_Packet& P, u16 type)
{
    co_await CExplosive::OnEvent(P, type);
    co_await inherited::OnEvent(P, type);
}

tmc::task<void> CExplosiveItem::UpdateCL()
{
    co_await CExplosive::UpdateCL();
    co_await inherited::UpdateCL();
}

tmc::task<void> CExplosiveItem::shedule_Update(u32 dt)
{
    co_await inherited::shedule_Update(dt);

    if (CDelayedActionFuse::isActive() && CDelayedActionFuse::Update(GetCondition()))
    {
        Fvector normal;
        FindNormal(normal);
        CExplosive::GenExplodeEvent(Position(), normal);
        CParticlesPlayer::StopParticles(ID(), BI_NONE, true);
    }
}

bool CExplosiveItem::shedule_Needed() { return inherited::shedule_Needed() || CDelayedActionFuse::isActive(); }

void CExplosiveItem::net_Relcase(CObject* O)
{
    CExplosive::net_Relcase(O);
    inherited::net_Relcase(O);
}

void CExplosiveItem::ActivateExplosionBox(const Fvector&, Fvector&) {}
void CExplosiveItem::GetRayExplosionSourcePos(Fvector& pos) { random_point_in_object_box(pos, this); }
