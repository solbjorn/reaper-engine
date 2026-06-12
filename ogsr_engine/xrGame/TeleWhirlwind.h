#ifndef TELE_WHIRLWIND
#define TELE_WHIRLWIND

#include "ai/monsters/telekinesis.h"
#include "ai/monsters/telekinetic_object.h"
#include "PHImpact.h"

class CTeleWhirlwind;
class CGameObject;

class CTeleWhirlwindObject : public CTelekineticObject
{
    RTTI_DECLARE_TYPEINFO(CTeleWhirlwindObject, CTelekineticObject);

public:
    typedef CTelekineticObject inherited;

    CTeleWhirlwind* m_telekinesis{};
    float throw_power{};
    bool b_destroyable{};

    CTeleWhirlwindObject();
    ~CTeleWhirlwindObject() override;

    [[nodiscard]] bool init(CTelekinesis* tele, CPhysicsShellHolder* obj, f32 s, f32 h, u32 ttk, bool rot = true) override;
    void set_throw_power(float throw_pow);
    [[nodiscard]] bool can_activate(CPhysicsShellHolder* obj) override;
    void raise() override;
    void raise_update() override;
    void keep() override;
    void release() override;
    void fire(const Fvector&, float) override;
    void switch_state(ETelekineticState new_state) override;
    [[nodiscard]] virtual bool destroy_object(const Fvector3 dir, f32 val);
};

class CTeleWhirlwind : public CTelekinesis
{
    RTTI_DECLARE_TYPEINFO(CTeleWhirlwind, CTelekinesis);

private:
    typedef CTelekinesis inherited;

    Fvector m_center{};
    float m_keep_radius{1.f};
    float m_throw_power{100.f};
    CGameObject* m_owner_object{};
    PH_IMPACT_STORAGE m_saved_impacts;
    shared_str m_destroying_particles;

public:
    CTeleWhirlwind();
    ~CTeleWhirlwind() override;

    CGameObject* OwnerObject() const { return m_owner_object; }
    const Fvector& Center() const { return m_center; }
    void SetCenter(const Fvector center) { m_center.set(center); }
    void SetOwnerObject(CGameObject* owner_object) { m_owner_object = owner_object; }
    void add_impact(const Fvector& dir, float val);
    void draw_out_impact(Fvector& dir, float& val);
    void clear_impacts();
    void set_destroing_particles(const shared_str& destroying_particles) { m_destroying_particles = destroying_particles; }
    const shared_str& destroing_particles() { return m_destroying_particles; }
    [[nodiscard]] CTelekineticObject* activate(CPhysicsShellHolder* obj, f32 strength, f32 height, u32 max_time_keep, bool rot = true) override;
    void clear() override;
    void clear_notrelevant() override;
    [[nodiscard]] CTelekineticObject* alloc_tele_object() override { return static_cast<CTelekineticObject*>(xr_new<CTeleWhirlwindObject>()); }
    [[nodiscard]] float keep_radius() { return m_keep_radius; }
    void set_throw_power(float throw_pow);
};

#endif
