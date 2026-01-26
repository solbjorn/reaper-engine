#pragma once

#include "physicsshellholder.h"

class CPHLeaderGeomShell;
class CPHCharacter;
struct dContact;
struct SGameMtl;

class CClimableObject : public CPhysicsShellHolder
{
    RTTI_DECLARE_TYPEINFO(CClimableObject, CPhysicsShellHolder);

public:
    typedef CPhysicsShellHolder inherited;

    CPHLeaderGeomShell* m_pStaticShell{};
    Fobb m_box{};
    Fvector m_axis;
    Fvector m_side;
    Fvector m_norm;
    float m_radius{};

    CClimableObject();
    ~CClimableObject() override;

    virtual void Load(LPCSTR section);
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    tmc::task<void> shedule_Update(u32 dt) override; // Called by sheduler
    tmc::task<void> UpdateCL() override; // Called each frame, so no need for dt
    virtual void Center(Fvector& C) const;
    virtual float Radius() const;

    virtual void OnRender();

protected:
    virtual BOOL UsedAI_Locations();

public:
    const Fvector& Axis() const { return m_axis; }
    float DDAxis(Fvector& dir) const;

    const Fvector& Side() const { return m_side; }
    float DDSide(Fvector& dir) const;

    const Fvector& Norm() const { return m_norm; }
    float DDNorm(Fvector& dir) const;
    bool BeforeLadder(CPHCharacter* actor, float tolerance = 0.f) const;
    float DDLowerP(CPHCharacter* actor, Fvector& out_dir) const; // returns distance and dir to lover point
    float DDUpperP(CPHCharacter* actor, Fvector& out_dir) const; // returns distance and dir to upper point

    void DToAxis(CPHCharacter* actor, Fvector& dir) const;
    float DDToAxis(CPHCharacter* actor, Fvector& out_dir) const; // returns distance and dir to ladder axis
    void POnAxis(CPHCharacter* actor, Fvector& P) const;

    float AxDistToUpperP(CPHCharacter* actor) const;
    float AxDistToLowerP(CPHCharacter* actor) const;

    void DSideToAxis(CPHCharacter* actor, Fvector& dir) const;
    float DDSideToAxis(CPHCharacter* actor, Fvector& dir) const;

    void DToPlain(CPHCharacter* actor, Fvector& dist) const;
    float DDToPlain(CPHCharacter* actor, Fvector& dir) const;
    bool InRange(CPHCharacter* actor) const;
    bool InTouch(CPHCharacter* actor) const;

    void LowerPoint(Fvector& P) const;
    void UpperPoint(Fvector& P) const;
    static void ObjectContactCallback(bool&, bool bo1, dContact& c, SGameMtl*, SGameMtl*);

public:
    virtual bool register_schedule() const { return false; }
};
