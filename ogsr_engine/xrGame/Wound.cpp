// Wound.cpp: класс описания раны
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "wound.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "../xr_3da/bone.h"

CWound::CWound(u16 bone_num) : m_iBoneNum{bone_num}, m_iParticleBoneNum{BI_NONE} { m_Wounds.resize(ALife::eHitTypeMax, 0.0f); }

CWound::~CWound() = default;

namespace
{
constexpr float WOUND_MAX{10.0f};
}

// serialization
void CWound::save(NET_Packet& output_packet)
{
    output_packet.w_u8(m_iBoneNum == BI_NONE ? std::numeric_limits<u8>::max() : gsl::narrow_cast<u8>(m_iBoneNum));

    for (std::underlying_type_t<ALife::EHitType> i{}; i < ALife::eHitTypeMax; ++i)
        output_packet.w_float_q8(m_Wounds[i], 0.f, WOUND_MAX);
}

void CWound::load(IReader& input_packet)
{
    m_iBoneNum = input_packet.r_u8();
    if (m_iBoneNum == std::numeric_limits<u8>::max())
        m_iBoneNum = BI_NONE;

    for (std::underlying_type_t<ALife::EHitType> i{}; i < ALife::eHitTypeMax; ++i)
    {
        m_Wounds[i] = input_packet.r_float_q8(0.f, WOUND_MAX);
        VERIFY(m_Wounds[i] >= 0.0f && m_Wounds[i] <= WOUND_MAX);
    }
}

float CWound::TotalSize() const
{
    float total_size{};

    for (std::underlying_type_t<ALife::EHitType> i{}; i < ALife::eHitTypeMax; ++i)
    {
        VERIFY(_valid(m_Wounds[i]));
        total_size += m_Wounds[i];
    }

    VERIFY(_valid(total_size));

    return total_size;
}

float CWound::TypeSize(ALife::EHitType hit_type) const { return m_Wounds[hit_type]; }

// кол-во кровавых ран
float CWound::BloodSize() const { return m_Wounds[ALife::eHitTypeWound] + m_Wounds[ALife::eHitTypeFireWound]; }

void CWound::AddHit(float hit_power, ALife::EHitType hit_type)
{
    m_Wounds[hit_type] += hit_power;
    clamp(m_Wounds[hit_type], 0.0f, WOUND_MAX);
}

void CWound::Incarnation(float percent, float min_wound_size)
{
    if (fis_zero(TotalSize()))
    {
        for (std::underlying_type_t<ALife::EHitType> i{}; i < ALife::eHitTypeMax; ++i)
            m_Wounds[i] = 0.0f;

        return;
    }

    // заживить все раны пропорционально их размеру
    for (std::underlying_type_t<ALife::EHitType> i{}; i < ALife::eHitTypeMax; ++i)
    {
        m_Wounds[i] -= percent /* *m_Wounds[i]*/;
        if (m_Wounds[i] < min_wound_size)
            m_Wounds[i] = 0.0f;
    }
}
