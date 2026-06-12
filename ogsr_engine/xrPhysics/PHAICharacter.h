#pragma once

#include "PHSimpleCharacter.h"

class CPHAICharacter : public CPHSimpleCharacter
{
    RTTI_DECLARE_TYPEINFO(CPHAICharacter, CPHSimpleCharacter);

public:
    typedef CPHSimpleCharacter inherited;

    Fvector m_vDesiredPosition;
    bool m_forced_physics_control;

    CPHAICharacter();
    ~CPHAICharacter() override = default;

    [[nodiscard]] CPHAICharacter* CastAICharacter() override { return this; }
    void SetPosition(Fvector pos) override;
    void SetDesiredPosition(const Fvector& pos) override { m_vDesiredPosition.set(pos); }
    void GetDesiredPosition(Fvector& dpos) override { dpos.set(m_vDesiredPosition); }
    void ValidateWalkOn() override;
    [[nodiscard]] bool TryPosition(Fvector pos) override;
    void Jump(const Fvector& jump_velocity) override;
    void SetMaximumVelocity(dReal vel) override { m_max_velocity = vel; }
    void InitContact(dContact* c, bool& do_collide, u16 material_idx_1, u16 material_idx_2) override;
    void SetForcedPhysicsControl(bool v) override { m_forced_physics_control = v; }
    [[nodiscard]] bool ForcedPhysicsControl() override { return m_forced_physics_control; }
    void Create(dVector3 sizes) override;

#ifdef DEBUG
    virtual void OnRender();
#endif
};
