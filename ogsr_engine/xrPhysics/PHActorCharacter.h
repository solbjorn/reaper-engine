#pragma once

#include "phsimplecharacter.h"
#include "ExtendedGeom.h"

struct SPHCharacterRestrictor
{
    SPHCharacterRestrictor(CPHCharacter::ERestrictionType Ttype) : m_type{Ttype} {}
    ~SPHCharacterRestrictor() { Destroy(); }

    CPHCharacter* m_character{};
    dGeomID m_restrictor{};
    dGeomID m_restrictor_transform{};

    CPHCharacter::ERestrictionType m_type;
    float m_restrictor_radius{0.1f};

    void SetObjectContactCallback(ObjectContactCallbackFun* callback);
    void SetMaterial(u16 material);
    void Create(CPHCharacter* ch, dVector3 sizes);
    void Destroy();
    void SetPhysicsRefObject(CPhysicsShellHolder* ref_object);
    void SetRadius(float r);
};

template <CPHCharacter::ERestrictionType Ttype>
struct TPHCharacterRestrictor : public SPHCharacterRestrictor
{
    TPHCharacterRestrictor() : SPHCharacterRestrictor{Ttype} {}

    void Create(CPHCharacter*, dVector3) { dGeomUserDataSetObjectContactCallback(m_restrictor, RestrictorCallBack); }

    static void RestrictorCallBack(bool& do_colide, bool bo1, dContact& c, SGameMtl*, SGameMtl*)
    {
        do_colide = false;
        dBodyID b1 = dGeomGetBody(c.geom.g1);
        dBodyID b2 = dGeomGetBody(c.geom.g2);
        if (!(b1 && b2))
            return;
        dxGeomUserData* ud1 = retrieveGeomUserData(c.geom.g1);
        dxGeomUserData* ud2 = retrieveGeomUserData(c.geom.g2);
        if (!(ud1 && ud2))
            return;

        CPHObject* o1{};
        if (ud1)
            o1 = ud1->ph_object;

        CPHObject* o2{};
        if (ud2)
            o2 = ud2->ph_object;

        if (!(o1 && o2))
            return;

        if (o1->CastType() != CPHObject::tpCharacter || o2->CastType() != CPHObject::tpCharacter)
            return;

        CPHCharacter* ch1{static_cast<CPHCharacter*>(o1)};
        CPHCharacter* ch2{static_cast<CPHCharacter*>(o2)};

        if (bo1)
        {
            ch1->ChooseRestrictionType(Ttype, c.geom.depth, ch2);
            do_colide = ch2->TouchRestrictor(Ttype);
        }
        else
        {
            ch2->ChooseRestrictionType(Ttype, c.geom.depth, ch1);
            do_colide = ch1->TouchRestrictor(Ttype);
        }
    }
};

DEFINE_VECTOR(SPHCharacterRestrictor*, RESRICTORS_V, RESTRICTOR_I);

IC RESTRICTOR_I begin(RESRICTORS_V& v) { return v.begin(); }

IC RESTRICTOR_I end(RESRICTORS_V& v) { return v.end(); }

class CPHActorCharacter : public CPHSimpleCharacter
{
    RTTI_DECLARE_TYPEINFO(CPHActorCharacter, CPHSimpleCharacter);

public:
    typedef CPHSimpleCharacter inherited;

    RESRICTORS_V m_restrictors;
    float m_speed_goal{};

    typedef TPHCharacterRestrictor<CPHCharacter::rtStalker> stalker_restrictor;
    typedef TPHCharacterRestrictor<CPHCharacter::rtStalkerSmall> stalker_small_restrictor;
    typedef TPHCharacterRestrictor<CPHCharacter::rtMonsterMedium> medium_monster_restrictor;

    CPHActorCharacter();
    ~CPHActorCharacter() override;

    [[nodiscard]] CPHActorCharacter* CastActorCharacter() override { return this; }
    void SetObjectContactCallback(ObjectContactCallbackFun* callback) override;
    void SetMaterial(u16 material) override;
    void Create(dVector3 sizes) override;
    void Destroy() override;
    void SetPhysicsRefObject(CPhysicsShellHolder* ref_object) override;
    void SetAcceleration(Fvector accel) override;
    void Disable() override;
    void Jump(const Fvector& jump_velocity) override;
    void InitContact(dContact* c, bool& do_collide, u16 material_idx_1, u16 material_idx_2) override;
    void SetRestrictorRadius(CPHCharacter::ERestrictionType rtype, float r);
    void ChooseRestrictionType(ERestrictionType my_type, float my_depth, CPHCharacter* ch) override;

private:
    void ClearRestrictors();
    RESTRICTOR_I Restrictor(CPHCharacter::ERestrictionType rtype);
};
