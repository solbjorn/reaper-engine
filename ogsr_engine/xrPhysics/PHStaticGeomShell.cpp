#include "stdafx.h"

#include "PHStaticGeomShell.h"

#include "SpaceUtils.h"
#include "GameObject.h"
#include "PhysicsShellHolder.h"
#include "../Include/xrRender/Kinematics.h"
#include "PHCollideValidator.h"

CPHStaticGeomShell::CPHStaticGeomShell() { spatial.type |= STYPE_PHYSIC; }
CPHStaticGeomShell::~CPHStaticGeomShell() = default;

void CPHStaticGeomShell::get_spatial_params()
{
    Fvector AABB;
    spatialParsFromDGeom(dSpacedGeometry(), spatial.sphere.P, AABB, spatial.sphere.R);
}

void CPHStaticGeomShell::PhDataUpdate(dReal step)
{
    Island().Step(step);
    Island().Unmerge();
    PhysicsRefObject()->enable_notificate();
    CPHUpdateObject::Deactivate();
}

void CPHStaticGeomShell::Activate(const Fmatrix& form)
{
    build();
    setStaticForm(form);
    get_spatial_params();
    spatial_register();
}

void CPHStaticGeomShell::Deactivate()
{
    spatial_unregister();
    CPHUpdateObject::Deactivate();
    destroy();
}

namespace
{
void cb(CBoneInstance*) {}
} // namespace

void P_BuildStaticGeomShell(CPHStaticGeomShell* pUnbrokenObject, CGameObject* obj, ObjectContactCallbackFun* object_contact_callback, Fobb& b)
{
    pUnbrokenObject->add_Box(b);
    pUnbrokenObject->Activate(obj->XFORM());

    pUnbrokenObject->set_PhysicsRefObject(smart_cast<CPhysicsShellHolder*>(obj));
    // m_pUnbrokenObject->SetPhObjectInGeomData(m_pUnbrokenObject);
    pUnbrokenObject->set_ObjectContactCallback(object_contact_callback);
    CPHCollideValidator::SetNonDynamicObject(*pUnbrokenObject);
}

CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj, ObjectContactCallbackFun* object_contact_callback, Fobb& b)
{
    CPHStaticGeomShell* pUnbrokenObject = xr_new<CPHStaticGeomShell>();
    P_BuildStaticGeomShell(pUnbrokenObject, obj, object_contact_callback, b);
    return pUnbrokenObject;
}

CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj, ObjectContactCallbackFun* object_contact_callback)
{
    IRenderVisual* V = XR_ASSERT_VAL(obj->Visual() != nullptr);
    auto K = XR_ASSERT_VAL(smart_cast<IKinematics*>(V) != nullptr);
    Fobb b;

    K->CalculateBones(); //. bForce - was TRUE
    V->getVisData().box.getradius(b.m_halfsize);

    b.xform_set(Fidentity);
    CPHStaticGeomShell* pUnbrokenObject = P_BuildStaticGeomShell(obj, object_contact_callback, b);

    K->CalculateBones();

    for (u16 k = 0; k < K->LL_BoneCount(); k++)
        K->LL_GetBoneInstance(k).set_callback(bctPhysics, cb, K->LL_GetBoneInstance(k).callback_param(), TRUE);

    return pUnbrokenObject;
}
