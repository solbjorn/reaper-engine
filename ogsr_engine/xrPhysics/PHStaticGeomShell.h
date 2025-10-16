#ifndef PH_STATIC_GEOM_SHELL_H
#define PH_STATIC_GEOM_SHELL_H

#include "PHGeometryOwner.h"
#include "PHObject.h"

class CPHStaticGeomShell : public CPHGeometryOwner, public CPHObject, public CPHUpdateObject
{
    RTTI_DECLARE_TYPEINFO(CPHStaticGeomShell, CPHGeometryOwner, CPHObject, CPHUpdateObject);

public:
    CPHStaticGeomShell();

    void get_spatial_params();
    virtual void EnableObject(CPHObject*) { CPHUpdateObject::Activate(); }
    virtual dGeomID dSpacedGeom() { return dSpacedGeometry(); }
    virtual void PhDataUpdate(dReal step);
    virtual void PhTune(dReal) {}
    virtual void InitContact(dContact*, bool&, u16, u16) {}
    virtual u16 get_elements_number() { return 0; }
    virtual CPHSynchronize* get_element_sync(u16) { return nullptr; }

    void Activate(const Fmatrix& form);
    void Deactivate();
};

CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj, ObjectContactCallbackFun* object_contact_callback);
CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj, ObjectContactCallbackFun* object_contact_callback, Fobb& b);
void P_BuildStaticGeomShell(CPHStaticGeomShell* shell, CGameObject* obj, ObjectContactCallbackFun* object_contact_callback, Fobb& b);

#endif // PH_STATIC_GEOM_SHELL_H
