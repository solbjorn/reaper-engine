////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "restriction_space.h"
#include "..\xr_3da\feel_touch.h"

class CSpaceRestrictor : public CGameObject, public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(CSpaceRestrictor, CGameObject, Feel::Touch);

private:
    typedef CGameObject inherited;

    enum
    {
        PLANE_COUNT = 6,
    };

    typedef Fplane CPlanesArray[PLANE_COUNT];

    struct CPlanes
    {
        CPlanesArray m_planes;
    };

    typedef xr_vector<Fsphere> SPHERES;
    typedef xr_vector<CPlanes> BOXES;

    mutable SPHERES m_spheres;
    mutable BOXES m_boxes;
    Fsphere m_selfbounds{};
    bool m_actuality{false};

    u8 m_space_restrictor_type;

    IC void actual(bool value) { m_actuality = value; }
    void prepare();
    bool prepared_inside(const Fsphere& sphere) const;

public:
    inline CSpaceRestrictor();
    ~CSpaceRestrictor() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* data) override;
    tmc::task<void> net_Destroy() override;
    bool inside(const Fsphere& sphere);
    void Center(Fvector& C) const override;
    [[nodiscard]] f32 Radius() const override;
    [[nodiscard]] BOOL UsedAI_Locations() override;
    void spatial_move() override;
    IC bool actual() const;
    [[nodiscard]] CSpaceRestrictor* cast_restrictor() override { return this; }
    [[nodiscard]] bool register_schedule() const override { return false; }

    IC RestrictionSpace::ERestrictorTypes restrictor_type() const;
    IC void change_restrictor_type(RestrictionSpace::ERestrictorTypes);

    virtual void OnRender();

private:
    bool b_scheduled;

public:
    void ScheduleRegister();
    void ScheduleUnregister();
    IC bool IsScheduled() { return b_scheduled; }

    void net_Relcase(CObject*) override;
    tmc::task<void> shedule_Update(u32) override;
    void feel_touch_new(CObject*) override;
    void feel_touch_delete(CObject*) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject*) override;
    bool active_contact(u16) const;
    float distance_to(Fvector&);
};
XR_SOL_BASE_CLASSES(CSpaceRestrictor);

#include "space_restrictor_inline.h"
