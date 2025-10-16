#pragma once

#include "../../../xr_3da/feel_touch.h"
#include "energy_holder.h"

class CBaseMonster;

class CPsyAura : public Feel::Touch, public CEnergyHolder
{
    RTTI_DECLARE_TYPEINFO(CPsyAura, Feel::Touch, CEnergyHolder);

public:
    typedef CEnergyHolder inherited;

    // владелец поля
    CBaseMonster* m_object{};

    // радиус поля
    float m_radius{1.f};

    CPsyAura();
    virtual ~CPsyAura();

    void init_external(CBaseMonster* obj) { m_object = obj; }
    [[nodiscard]] BOOL feel_touch_contact(CObject*) override { return false; }
    virtual void schedule_update();
    virtual void process_objects_in_aura() {}

    // свойства поля
    void set_radius(float R) { m_radius = R; }
    float get_radius() { return m_radius; }

    CBaseMonster* get_object() { return m_object; }
};
