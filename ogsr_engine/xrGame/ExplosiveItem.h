//////////////////////////////////////////////////////////////////////
// ExplosiveItem.h: класс для вещи которая взрывается под
//					действием различных хитов (канистры,
//					балоны с газом и т.д.)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Explosive.h"
#include "inventory_item_object.h"
#include "DelayedActionFuse.h"

class CExplosiveItem : public CInventoryItemObject, public CDelayedActionFuse, public CExplosive
{
    RTTI_DECLARE_TYPEINFO(CExplosiveItem, CInventoryItemObject, CDelayedActionFuse, CExplosive);

private:
    typedef CInventoryItemObject inherited;

public:
    CExplosiveItem();
    ~CExplosiveItem() override;

    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override { co_return co_await CInventoryItemObject::net_Spawn(DC); }
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override { CInventoryItemObject::net_Export(E); }
    void net_Relcase(CObject* O) override;
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }
    [[nodiscard]] CExplosive* cast_explosive() override { return this; }
    [[nodiscard]] IDamageSource* cast_IDamageSource() override { return CExplosive::cast_IDamageSource(); }
    void GetRayExplosionSourcePos(Fvector3& pos) override;
    void ActivateExplosionBox(const Fvector&, Fvector&) override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    void Hit(SHit* pHDS) override;
    tmc::task<void> shedule_Update(u32 dt) override;
    [[nodiscard]] bool shedule_Needed() override;

    tmc::task<void> UpdateCL() override;
    void ChangeCondition(f32 fDeltaCondition) override { CInventoryItem::ChangeCondition(fDeltaCondition); }
    void StartTimerEffects() override;
};
