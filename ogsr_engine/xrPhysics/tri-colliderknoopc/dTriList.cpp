#include "stdafx.h"

#include "dTriCollideK.h"
#include "dxTriList.h"
#include "dcTriListCollider.h"
#include "../ExtendedGeom.h"
#include "dcTriListCollider.cpp" // Allow inlining
#include "gameobject.h"

int dTriListClass = -1;

namespace
{
dcTriListCollider* GetData(dxGeom* TriList)
{
    dxTriList* Data = (dxTriList*)dGeomGetClassData(TriList);

    return Data->Collider;
}

inline bool ValidateCollision(dxGeom* o1) { return dGeomGetUserData(o1)->b_static_colide; }

int dCollideSTL(dxGeom* TriList, dxGeom* Sphere, int Flags, dContactGeom* Contact, int Stride) noexcept
{
    if (ValidateCollision(Sphere))
        return GetData(TriList)->CollideSphere(Sphere, Flags, Contact, Stride);

    return 0;
}

int dCollideBTL(dxGeom* TriList, dxGeom* Box, int Flags, dContactGeom* Contact, int Stride) noexcept
{
    if (ValidateCollision(Box))
        return GetData(TriList)->CollideBox(Box, Flags, Contact, Stride);

    return 0;
}

int dCollideCTL(dxGeom* TriList, dxGeom* Cyl, int Flags, dContactGeom* Contact, int Stride) noexcept
{
    if (ValidateCollision(Cyl))
        return GetData(TriList)->CollideCylinder(Cyl, Flags, Contact, Stride);

    return 0;
}

dColliderFn* dTriListColliderFn(int num)
{
    if (num == dBoxClass)
        return (dColliderFn*)&dCollideBTL;

    if (num == dSphereClass)
        return (dColliderFn*)&dCollideSTL;

    if (num == dCylinderClassUser)
        return (dColliderFn*)&dCollideCTL;

    return nullptr;
}

int dAABBTestTL(dxGeom*, dxGeom*, dReal[6]) noexcept { return 1; }

void dDestroyTriList(dGeomID g) { xr_delete(((dxTriList*)dGeomGetClassData(g))->Collider); }
} // namespace

/* External functions */

dxGeom* dCreateTriList(dSpaceID space, dTriCallback* Callback, dTriArrayCallback* ArrayCallback)
{
    if (dTriListClass == -1)
    {
        dGeomClass c;

        c.bytes = sizeof(dxTriList);

        c.collider = &dTriListColliderFn;

        c.aabb = &dInfiniteAABB;

        c.aabb_test = &dAABBTestTL;

        //	c.aabb_test=NULL;
        c.dtor = &dDestroyTriList;

        dTriListClass = dCreateGeomClass(&c);
    }

    dxGeom* g = dCreateGeom(dTriListClass);

    if (space)
        dSpaceAdd(space, g);

    dxTriList* Data = (dxTriList*)dGeomGetClassData(g);

    Data->Callback = Callback;

    Data->ArrayCallback = ArrayCallback;

    Data->Collider = xr_new<dcTriListCollider>(g);

    return g;
}
