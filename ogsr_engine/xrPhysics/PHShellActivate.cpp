#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
#include "PHCollideValidator.h"
#include "Level.h"
#include "physicsshellholder.h"

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
#include "PhysicsShellAnimator.h"
#endif

///////////////////////////////////////////////////////////////
/// #pragma warning(disable:4995)
// #include "ode/src/collision_kernel.h"
// #include "ode/src/joint.h"
// #include "ode/src/objects.h"

// #pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHShell.h"
void CPHShell::activate(bool disable)
{
    PresetActive();
    if (!CPHObject::is_active())
        vis_update_deactivate();
    if (!disable)
        EnableObject(0);
}
void CPHShell::Activate(const Fmatrix& m0, float dt01, const Fmatrix& m2, bool disable)
{
    if (isActive())
        return;
    activate(disable);
    //	ELEMENT_I i;
    mXFORM.set(m0);
    // for(i=elements.begin();elements.end() != i;++i){

    //	(*i)->Activate(m0,dt01, m2, disable);
    //}

    {
        ELEMENT_I i = elements.begin(), e = elements.end();
        for (; i != e; ++i)
            (*i)->Activate(mXFORM, disable);
    }

    {
        JOINT_I i = joints.begin(), e = joints.end();
        for (; i != e; ++i)
            (*i)->Activate();
    }

    Fmatrix m;
    GetGlobalTransformDynamic(&m);
    m.invert();
    m.mulA_43(mXFORM);
    TransformPosition(m);
    if (PKinematics())
    {
        SetCallbacks(GetBonesCallback());
    }

    // bActive=true;
    // bActivating=true;
    m_flags.set(flActive, TRUE);
    m_flags.set(flActivating, TRUE);
    spatial_register();
    ///////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    // mXFORM.set(m0);
    // Activate(disable);
    Fvector lin_vel;
    lin_vel.sub(m2.c, m0.c);
    set_LinearVel(lin_vel);
}

void CPHShell::Activate(const Fmatrix& transform, const Fvector& lin_vel, const Fvector& ang_vel, bool disable)
{
    if (isActive())
        return;
    activate(disable);

    ELEMENT_I i;
    mXFORM.set(transform);
    for (i = elements.begin(); elements.end() != i; ++i)
    {
        (*i)->Activate(transform, lin_vel, ang_vel);
    }

    {
        JOINT_I i = joints.begin(), e = joints.end();
        for (; i != e; ++i)
            (*i)->Activate();
    }

    if (PKinematics())
    {
        SetCallbacks(GetBonesCallback());
    }
    spatial_register();
    // bActive=true;
    // bActivating=true;
    m_flags.set(flActivating, TRUE);
    m_flags.set(flActive, TRUE);
    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    // mXFORM.set(transform);
    // Activate(disable);
    // set_LinearVel(lin_vel);
    // set_AngularVel(ang_vel);
}

void CPHShell::Activate(bool disable, bool not_set_bone_callbacks)
{
    if (isActive())
        return;

    activate(disable);
    {
        IKinematics* K = m_pKinematics;
        if (not_set_bone_callbacks)
            m_pKinematics = 0;

        ELEMENT_I i = elements.begin(), e = elements.end();
        for (; i != e; ++i)
            (*i)->Activate(mXFORM, disable);

        m_pKinematics = K;
    }

    {
        JOINT_I i = joints.begin(), e = joints.end();
        for (; i != e; ++i)
            (*i)->Activate();
    }

    if (PKinematics() && !not_set_bone_callbacks)
    {
        SetCallbacks(GetBonesCallback());
    }
    spatial_register();
    m_flags.set(flActivating, TRUE);
    m_flags.set(flActive, TRUE);
}

void CPHShell::Build(bool disable /*false*/)
{
    if (isActive())
        return;

    PresetActive();
    m_flags.set(flActivating, TRUE);
    m_flags.set(flActive, TRUE);

    {
        ELEMENT_I i = elements.begin(), e = elements.end();
        for (; i != e; ++i)
        {
            (*i)->build(disable);
        }
    }

    {
        JOINT_I i = joints.begin(), e = joints.end();
        for (; i != e; ++i)
            (*i)->Create();
    }
}

void CPHShell::RunSimulation(bool place_current_forms /*true*/)
{
    if (!CPHObject::is_active())
        vis_update_deactivate();
    EnableObject(0);

    dSpaceSetCleanup(m_space, 0);

    {
        ELEMENT_I i = elements.begin(), e = elements.end();
        if (place_current_forms)
            for (; i != e; ++i)
                (*i)->RunSimulation(mXFORM);
    }
    {
        JOINT_I i = joints.begin(), e = joints.end();
        for (; i != e; ++i)
            (*i)->RunSimulation();
    }

    spatial_register();
}

void CPHShell::AfterSetActive()
{
    if (isActive())
        return;
    PureActivate();
    // bActive=true;
    m_flags.set(flActive, TRUE);
    ELEMENT_I i = elements.begin(), e = elements.end();
    for (; i != e; ++i)
        (*i)->PresetActive();
}

void CPHShell::PureActivate()
{
    if (isActive())
        return;
    // bActive=true;
    m_flags.set(flActive, TRUE);
    if (!CPHObject::is_active())
        vis_update_deactivate();
    EnableObject(0);
    m_object_in_root.identity();
    spatial_register();
}

void CPHShell::PresetActive()
{
    VERIFY(!isActive());
    if (!m_space)
    {
        m_space = dSimpleSpaceCreate(0);
        dSpaceSetCleanup(m_space, 0);
    }
}

void CPHShell::Deactivate()
{
#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    if (m_pPhysicsShellAnimatorC)
    {
        xr_delete(m_pPhysicsShellAnimatorC);
    }
#endif

    if (!isActive())
        return;
    R_ASSERT2(!ph_world->Processing(), "can not deactivate physics shell during physics processing!!!");
    R_ASSERT2(!ph_world->IsFreezed(), "can not deactivate physics shell when ph world is freezed!!!");
    R_ASSERT2(!CPHObject::IsFreezed(), "can not deactivate freezed !!!");
    ZeroCallbacks();
    VERIFY(ph_world && ph_world->Exist());
    if (isFullActive())
    {
        vis_update_deactivate();
        CPHObject::activate();
        ph_world->Freeze();
        CPHObject::UnFreeze();
        ph_world->StepTouch();
        ph_world->UnFreeze();
        // Fmatrix m;
        // InterpolateGlobalTransform(&m);
    }
    spatial_unregister();

    vis_update_activate();
    // if(ref_object && !CPHObject::is_active() && m_active_count == 0)
    //{
    //	ref_object->processing_activate();
    // }
    DisableObject();
    CPHObject::remove_from_recently_deactivated();

    ELEMENT_I i;
    for (i = elements.begin(); elements.end() != i; ++i)
        (*i)->Deactivate();

    JOINT_I j;
    for (j = joints.begin(); joints.end() != j; ++j)
        (*j)->Deactivate();

    if (m_space)
    {
        dSpaceDestroy(m_space);
        m_space = NULL;
    }
    // bActive=false;
    // bActivating=false;
    m_flags.set(flActivating, FALSE);
    m_flags.set(flActive, FALSE);
    m_traced_geoms.clear();
    CPHObject::UnsetRayMotions();
}