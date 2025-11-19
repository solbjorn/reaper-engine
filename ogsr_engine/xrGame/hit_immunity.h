// hit_immunity.h: класс для тех объектов, которые поддерживают
//				   коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "script_export_space.h"
#include "hit_immunity_space.h"

class CHitImmunity : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CHitImmunity);

public:
    CHitImmunity();
    ~CHitImmunity() override;

    virtual void LoadImmunities(LPCSTR section, CInifile* ini);

    [[nodiscard]] float GetHitImmunity(ALife::EHitType hit_type) const { return m_HitTypeK[hit_type]; }
    [[nodiscard]] virtual float AffectHit(float power, ALife::EHitType hit_type);

protected:
    // коэффициенты на которые домножается хит
    // при соответствующем типе воздействия
    //(для защитных костюмов и специфичных животных)
    HitImmunity::HitTypeSVec m_HitTypeK;

public:
    [[nodiscard]] HitImmunity::HitTypeSVec& immunities() { return m_HitTypeK; }
    [[nodiscard]] const HitImmunity::HitTypeSVec& immunities() const { return m_HitTypeK; }

    [[nodiscard]] virtual CHitImmunity* cast_hit_immunities() { return this; }
    [[nodiscard]] virtual const CHitImmunity* cast_hit_immunities() const { return this; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
