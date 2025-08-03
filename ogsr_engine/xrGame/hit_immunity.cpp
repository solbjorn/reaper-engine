// hit_immunity.cpp:	класс для тех объектов, которые поддерживают
//						коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hit_immunity.h"
#include "GameObject.h"

CHitImmunity::CHitImmunity()
{
    m_HitTypeK.resize(ALife::eHitTypeMax);
    for (int i = 0; i < ALife::eHitTypeMax; i++)
        m_HitTypeK[i] = 1.0f;
}

CHitImmunity::~CHitImmunity() {}
void CHitImmunity::LoadImmunities(LPCSTR imm_sect, CInifile* ini)
{
    R_ASSERT2(ini->section_exist(imm_sect), imm_sect);

    m_HitTypeK[ALife::eHitTypeBurn] = ini->r_float(imm_sect, "burn_immunity");
    m_HitTypeK[ALife::eHitTypeStrike] = ini->r_float(imm_sect, "strike_immunity");
    m_HitTypeK[ALife::eHitTypeShock] = ini->r_float(imm_sect, "shock_immunity");
    m_HitTypeK[ALife::eHitTypeWound] = ini->r_float(imm_sect, "wound_immunity");
    m_HitTypeK[ALife::eHitTypeRadiation] = ini->r_float(imm_sect, "radiation_immunity");
    m_HitTypeK[ALife::eHitTypeTelepatic] = ini->r_float(imm_sect, "telepatic_immunity");
    m_HitTypeK[ALife::eHitTypeChemicalBurn] = ini->r_float(imm_sect, "chemical_burn_immunity");
    m_HitTypeK[ALife::eHitTypeExplosion] = ini->r_float(imm_sect, "explosion_immunity");
    m_HitTypeK[ALife::eHitTypeFireWound] = ini->r_float(imm_sect, "fire_wound_immunity");
    m_HitTypeK[ALife::eHitTypePhysicStrike] = READ_IF_EXISTS(ini, r_float, imm_sect, "physic_strike_wound_immunity", 1.0f);
    m_HitTypeK[ALife::eHitTypeWound_2] = READ_IF_EXISTS(ini, r_float, imm_sect, "wound_2_immunity", 1.0f);
}

float CHitImmunity::AffectHit(float power, ALife::EHitType hit_type) { return power * m_HitTypeK[hit_type]; }

static float get_burn_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeBurn]; }
static void set_burn_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeBurn] = i; }

static float get_strike_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeStrike]; }
static void set_strike_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeStrike] = i; }

static float get_shock_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeShock]; }
static void set_shock_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeShock] = i; }

static float get_wound_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeWound]; }
static void set_wound_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeWound] = i; }

static float get_radiation_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeRadiation]; }
static void set_radiation_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeRadiation] = i; }

static float get_telepatic_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeTelepatic]; }
static void set_telepatic_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeTelepatic] = i; }

static float get_chemical_burn_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeChemicalBurn]; }
static void set_chemical_burn_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeChemicalBurn] = i; }

static float get_explosion_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeExplosion]; }
static void set_explosion_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeExplosion] = i; }

static float get_fire_wound_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeFireWound]; }
static void set_fire_wound_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeFireWound] = i; }

static float get_wound_2_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeWound_2]; }
static void set_wound_2_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeWound_2] = i; }

static float get_physic_strike_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypePhysicStrike]; }
static void set_physic_strike_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypePhysicStrike] = i; }

void CHitImmunity::script_register(sol::state_view& lua)
{
    lua.new_usertype<CHitImmunity>(
        "CHitImmunity", sol::no_constructor, "burn_immunity", sol::property(&get_burn_immunity, &set_burn_immunity), "strike_immunity",
        sol::property(&get_strike_immunity, &set_strike_immunity), "shock_immunity", sol::property(&get_shock_immunity, &set_shock_immunity), "wound_immunity",
        sol::property(&get_wound_immunity, &set_wound_immunity), "radiation_immunity", sol::property(&get_radiation_immunity, &set_radiation_immunity), "telepatic_immunity",
        sol::property(&get_telepatic_immunity, &set_telepatic_immunity), "chemical_burn_immunity", sol::property(&get_chemical_burn_immunity, &set_chemical_burn_immunity),
        "explosion_immunity", sol::property(&get_explosion_immunity, &set_explosion_immunity), "fire_wound_immunity",
        sol::property(&get_fire_wound_immunity, &set_fire_wound_immunity), "wound_2_immunity", sol::property(&get_wound_2_immunity, &set_wound_2_immunity),
        "physic_strike_immunity", sol::property(&get_physic_strike_immunity, &set_physic_strike_immunity));
}
