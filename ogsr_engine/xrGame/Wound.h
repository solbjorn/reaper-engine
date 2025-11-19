// Wound.h: класс описания раны
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "hit_immunity_space.h"

class NET_Packet;

class CWound : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CWound);

public:
    explicit CWound(u16 bone_num);
    ~CWound() override;

    // serialization
    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);

    [[nodiscard]] float TotalSize() const;
    [[nodiscard]] float TypeSize(ALife::EHitType hit_type) const;
    [[nodiscard]] float BloodSize() const;

    void AddHit(float hit_power, ALife::EHitType hit_type);

    // заживление раны
    void Incarnation(float percent, float min_wound_size);
    [[nodiscard]] u16 GetBoneNum() const { return m_iBoneNum; }
    void SetBoneNum(u16 bone_num) { m_iBoneNum = bone_num; }

    [[nodiscard]] u16 GetParticleBoneNum() const { return m_iParticleBoneNum; }
    void SetParticleBoneNum(u16 bone_num) { m_iParticleBoneNum = bone_num; }

    [[nodiscard]] const shared_str& GetParticleName() const { return m_sParticleName; }
    void SetParticleName(shared_str particle_name) { m_sParticleName = particle_name; }

    void SetDestroy(bool destroy) { m_bToBeDestroy = destroy; }
    [[nodiscard]] bool GetDestroy() const { return m_bToBeDestroy; }

    // время обновления (для капель крови)
    float m_fDropTime{};

protected:
    // косточка на которой появилась рана
    u16 m_iBoneNum;

    // косточка, если на ране отыгрывается партикл
    u16 m_iParticleBoneNum;
    // имя этого партикла
    shared_str m_sParticleName;

    // список составляющих раны
    HitImmunity::HitTypeSVec m_Wounds;

    bool m_bToBeDestroy{};
};
