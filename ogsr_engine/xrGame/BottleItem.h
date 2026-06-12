///////////////////////////////////////////////////////////////
// BottleItem.h
// BottleItem - бутылка с напитком, которую можно разбить
///////////////////////////////////////////////////////////////

#pragma once

#include "fooditem.h"

class CBottleItem : public CFoodItem
{
    RTTI_DECLARE_TYPEINFO(CBottleItem, CFoodItem);

private:
    typedef CFoodItem inherited;

public:
    CBottleItem();
    ~CBottleItem() override;

    void Load(gsl::czstring section) override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

    void Hit(SHit* pHDS) override;

    void BreakToPieces();
    void UseBy(CEntityAlive* entity_alive) override;
    void ZeroAllEffects() override;

protected:
    float m_alcohol;
    // партиклы разбивания бутылки
    shared_str m_sBreakParticles;
    ref_sound sndBreaking;
};
