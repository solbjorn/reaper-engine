#ifndef PH_STATIC_GEOM_SHELL_H
#define PH_STATIC_GEOM_SHELL_H

#include "PHGeometryOwner.h"
#include "PHObject.h"

class CPHStaticGeomShell : public CPHGeometryOwner, public CPHObject, public CPHUpdateObject
{
    RTTI_DECLARE_TYPEINFO(CPHStaticGeomShell, CPHGeometryOwner, CPHObject, CPHUpdateObject);

public:
    CPHStaticGeomShell();
    ~CPHStaticGeomShell() override;

    void get_spatial_params() override;
    void EnableObject(CPHObject*) override { CPHUpdateObject::Activate(); }
    [[nodiscard]] dGeomID dSpacedGeom() override { return dSpacedGeometry(); }
    void PhDataUpdate(dReal step) override;
    void PhTune(dReal) override {}
    void InitContact(dContact*, bool&, u16, u16) override {}
    [[nodiscard]] u16 get_elements_number() override { return 0; }
    [[nodiscard]] CPHSynchronize* get_element_sync(u16) override { return nullptr; }

    void Activate(const Fmatrix& form);
    void Deactivate();
};

CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj, ObjectContactCallbackFun* object_contact_callback);
CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj, ObjectContactCallbackFun* object_contact_callback, Fobb& b);
void P_BuildStaticGeomShell(CPHStaticGeomShell* shell, CGameObject* obj, ObjectContactCallbackFun* object_contact_callback, Fobb& b);

#endif // PH_STATIC_GEOM_SHELL_H
