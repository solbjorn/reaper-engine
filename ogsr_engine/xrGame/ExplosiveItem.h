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

    virtual void Load(LPCSTR section);
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override { co_return co_await CInventoryItemObject::net_Spawn(DC); }
    tmc::task<void> net_Destroy() override;
    virtual void net_Export(CSE_Abstract* E) { CInventoryItemObject::net_Export(E); }
    virtual void net_Relcase(CObject* O);
    virtual CGameObject* cast_game_object() { return this; }
    virtual CExplosive* cast_explosive() { return this; }
    virtual IDamageSource* cast_IDamageSource() { return CExplosive::cast_IDamageSource(); }
    virtual void GetRayExplosionSourcePos(Fvector& pos);
    void ActivateExplosionBox(const Fvector&, Fvector&) override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    virtual void Hit(SHit* pHDS);
    tmc::task<void> shedule_Update(u32 dt) override;
    virtual bool shedule_Needed();

    tmc::task<void> UpdateCL() override;
    virtual void ChangeCondition(float fDeltaCondition) { CInventoryItem::ChangeCondition(fDeltaCondition); }
    virtual void StartTimerEffects();
};
