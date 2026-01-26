#include "stdafx.h"

#include "physicsskeletonobject.h"

#include "PhysicsShell.h"
#include "phsynchronize.h"
#include "xrserver_objects_alife.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xr_3da/xr_collide_form.h"

CPhysicsSkeletonObject::CPhysicsSkeletonObject() {}
CPhysicsSkeletonObject::~CPhysicsSkeletonObject() {}

tmc::task<bool> CPhysicsSkeletonObject::net_Spawn(CSE_Abstract* DC)
{
    CSE_Abstract* e = (CSE_Abstract*)(DC);

    std::ignore = co_await inherited::net_Spawn(DC);
    xr_delete(collidable.model);

    collidable.model = xr_new<CCF_Skeleton>(this);
    std::ignore = CPHSkeleton::Spawn(e);

    setVisible(TRUE);
    setEnabled(TRUE);

    if (!PPhysicsShell()->isBreakable())
        SheduleUnregister();

    co_return true;
}

void CPhysicsSkeletonObject::SpawnInitPhysics(CSE_Abstract* D)
{
    CreatePhysicsShell(D);

    IKinematics* K = smart_cast<IKinematics*>(Visual());
    if (K)
    {
        K->CalculateBones_Invalidate();
        K->CalculateBones();
    }
}

tmc::task<void> CPhysicsSkeletonObject::net_Destroy()
{
    co_await inherited::net_Destroy();
    CPHSkeleton::RespawnInit();
}

void CPhysicsSkeletonObject::Load(LPCSTR section)
{
    inherited::Load(section);
    CPHSkeleton::Load(section);
}

void CPhysicsSkeletonObject::CreatePhysicsShell(CSE_Abstract* e)
{
    if (m_pPhysicsShell)
        return;
    if (!Visual())
        return;

    CSE_PHSkeleton* po = smart_cast<CSE_PHSkeleton*>(e);
    m_pPhysicsShell = P_build_Shell(this, !po->_flags.test(CSE_PHSkeleton::flActive));
}

tmc::task<void> CPhysicsSkeletonObject::shedule_Update(u32 dt)
{
    co_await inherited::shedule_Update(dt);
    CPHSkeleton::Update(dt);
}

void CPhysicsSkeletonObject::net_Save(NET_Packet& P)
{
    inherited::net_Save(P);
    CPHSkeleton::SaveNetState(P);
}

BOOL CPhysicsSkeletonObject::net_SaveRelevant()
{
    return TRUE; //! m_flags.test(CSE_ALifeObjectPhysic::flSpawnCopy);
}

BOOL CPhysicsSkeletonObject::UsedAI_Locations() { return (FALSE); }

tmc::task<void> CPhysicsSkeletonObject::UpdateCL()
{
    co_await inherited::UpdateCL();

    PHObjectPositionUpdate();
}

void CPhysicsSkeletonObject::PHObjectPositionUpdate()
{
    if (m_pPhysicsShell)
        m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
}
