#pragma once

#include "CharacterPhysicsSupport.h"
#include "PhysicsCommon.h"
#include "ExtendedGeom.h"

// this is equivalent dMULTIPLYOP0_333 whith consequent transposion of A
#define dMULTIPLYOP3_333(A, op, B, C) \
    (A)[0] op dDOT14((B), (C)); \
    (A)[1] op dDOT14((B + 4), (C)); \
    (A)[2] op dDOT14((B + 8), (C)); \
    (A)[4] op dDOT14((B), (C + 1)); \
    (A)[5] op dDOT14((B + 4), (C + 1)); \
    (A)[6] op dDOT14((B + 8), (C + 1)); \
    (A)[8] op dDOT14((B), (C + 2)); \
    (A)[9] op dDOT14((B + 4), (C + 2)); \
    (A)[10] op dDOT14((B + 8), (C + 2))

inline void dMULTIPLY3_333(dReal* A, const dReal* B, const dReal* C) { dMULTIPLYOP3_333(A, =, B, C); }

class CGameObject;
class CPHObject;

#include "../xr_3da/IPhysicsDefinitions.h"

class XR_NOVTABLE CODEGeom : public IPhysicsGeometry
{
    RTTI_DECLARE_TYPEINFO(CODEGeom, IPhysicsGeometry);

protected:
    dGeomID m_geom_transform{};
    u16 m_bone_id{std::numeric_limits<u16>::max()};
    Flags16 m_flags{};

public:
    // get
    [[nodiscard]] virtual f32 volume() = 0;
    virtual void get_mass(dMass& m) = 0; // unit dencity mass;
    void get_mass(dMass& m, const Fvector& ref_point, float density);
    void get_mass(dMass& m, const Fvector& ref_point);
    void add_self_mass(dMass& m, const Fvector& ref_point);
    void add_self_mass(dMass& m, const Fvector& ref_point, float density);

    void get_local_center_bt(Fvector& center); // for built
    void get_global_center_bt(Fvector& center); // for built
    void get_local_form_bt(Fmatrix& form); // for built
    void get_global_form_bt(Fmatrix& form); // for built
    virtual void get_xform(Fmatrix& form);

    void get_Box(Fmatrix& form, Fvector& sz) override;
    bool collide_fluids() override;

    void set_static_ref_form(const Fmatrix& form); // for built
    virtual void get_max_area_dir_bt(Fvector& dir) = 0;
    [[nodiscard]] virtual f32 radius() = 0;
    virtual void get_extensions_bt(const Fvector& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) = 0;
    void clear_cashed_tries();
    IC dGeomID geom() { return dGeomTransformGetGeom(m_geom_transform); }
    IC dGeomID geometry_transform() { return m_geom_transform; }
    IC dGeomID geometry() { return m_geom_transform ? (geom() ? geom() : m_geom_transform) : nullptr; }

    IC dGeomID geometry_bt()
    {
        if (is_transformed_bt())
            return geom();
        else
            return geometry_transform();
    }

    ICF static bool is_transform(dGeomID g) { return dGeomGetClass(g) == dGeomTransformClass; }
    IC bool is_transformed_bt() { return is_transform(m_geom_transform); }
    IC u16& element_position() { return dGeomGetUserData(geometry())->element_position; }
    [[nodiscard]] virtual const Fvector& local_center() = 0;
    virtual void get_local_form(Fmatrix& form) = 0;
    virtual void set_local_form(const Fmatrix& form) = 0;
    // set
    // element part
    void set_body(dBodyID body);
    void set_bone_id(u16 id) { m_bone_id = id; }
    u16 bone_id() { return m_bone_id; }
    void set_shape_flags(const Flags16& _flags) { m_flags = _flags; }
    void add_to_space(dSpaceID space);
    void remove_from_space(dSpaceID space);
    void set_material(u16 ul_material);
    void set_contact_cb(ContactCallbackFun* ccb);
    void set_obj_contact_cb(ObjectContactCallbackFun* occb);
    void add_obj_contact_cb(ObjectContactCallbackFun* occb);
    void remove_obj_contact_cb(ObjectContactCallbackFun* occb);
    void set_callback_data(void* cd);
    void* get_callback_data();
    void set_ref_object(CPhysicsShellHolder* ro);
    void set_ph_object(CPHObject* o);

    // build/destroy
protected:
    void init();
    void get_final_tx_bt(const dReal*& p, const dReal*& R, dReal* bufV, dReal* bufM);
    [[nodiscard]] virtual dGeomID create() = 0;

public:
    static void get_final_tx(dGeomID g, const dReal*& p, const dReal*& R, dReal* bufV, dReal* bufM);
    void build(const Fvector& ref_point);
    virtual void set_position(const Fvector& ref_point); // for build geom
    void move_local_basis(const Fmatrix& inv_new_mul_old);
    void destroy();

    CODEGeom() = default;
    ~CODEGeom() override;
};

class CBoxGeom : public CODEGeom
{
    RTTI_DECLARE_TYPEINFO(CBoxGeom, CODEGeom);

public:
    typedef CODEGeom inherited;
    Fobb m_box;

    explicit CBoxGeom(const Fobb& box);
    ~CBoxGeom() override = default;

    [[nodiscard]] f32 volume() override;
    [[nodiscard]] f32 radius() override;
    void get_extensions_bt(const Fvector& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) override;
    void get_max_area_dir_bt(Fvector& dir) override;
    void get_mass(dMass& m) override; // unit dencity mass;
    [[nodiscard]] const Fvector& local_center() override;
    void get_local_form(Fmatrix& form) override;
    void set_local_form(const Fmatrix& form) override;
    [[nodiscard]] dGeomID create() override;
    void set_position(const Fvector& ref_point) override;
};

class CSphereGeom : public CODEGeom
{
    RTTI_DECLARE_TYPEINFO(CSphereGeom, CODEGeom);

public:
    typedef CODEGeom inherited;
    Fsphere m_sphere;

    explicit CSphereGeom(const Fsphere& sphere);
    ~CSphereGeom() override = default;

    [[nodiscard]] f32 volume() override;
    [[nodiscard]] f32 radius() override;
    void get_extensions_bt(const Fvector& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) override;
    void get_max_area_dir_bt(Fvector&) override {}
    void get_mass(dMass& m) override; // unit dencity mass;
    [[nodiscard]] const Fvector& local_center() override;
    void get_local_form(Fmatrix& form) override;
    void set_local_form(const Fmatrix& form) override;
    [[nodiscard]] dGeomID create() override;
    void set_position(const Fvector& ref_point) override;
};

class CCylinderGeom : public CODEGeom
{
    RTTI_DECLARE_TYPEINFO(CCylinderGeom, CODEGeom);

public:
    typedef CODEGeom inherited;
    Fcylinder m_cylinder;

    explicit CCylinderGeom(const Fcylinder& cyl);
    ~CCylinderGeom() override = default;

    [[nodiscard]] f32 volume() override;
    [[nodiscard]] f32 radius() override;
    void get_extensions_bt(const Fvector& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) override;
    void get_max_area_dir_bt(Fvector&) override {}
    [[nodiscard]] const Fvector& local_center() override;
    void get_mass(dMass& m) override; // unit dencity mass;
    void get_local_form(Fmatrix& form) override;
    void set_local_form(const Fmatrix& form) override;
    [[nodiscard]] dGeomID create() override;
    void set_position(const Fvector& ref_point) override;
};
