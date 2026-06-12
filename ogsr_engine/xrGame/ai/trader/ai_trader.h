////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_trader.h
//	Created 	: 16.04.2003
//  Modified 	: 16.04.2003
//	Author		: Jim
//	Description : Trader class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../../inventoryowner.h"
#include "../../script_entity.h"
#include "../../sound_player.h"
#include "../../AI_PhraseDialogManager.h"
#include "script_export_space.h"

class CInventoryItem;
class CArtefact;
class CMotionDef;
class CBlend;
class CSoundPlayer;
class CTraderAnimation;

class CAI_Trader : public CEntityAlive, public CInventoryOwner, public CScriptEntity, public CAI_PhraseDialogManager
{
    RTTI_DECLARE_TYPEINFO(CAI_Trader, CEntityAlive, CInventoryOwner, CScriptEntity, CAI_PhraseDialogManager);

public:
    typedef CEntityAlive inherited;

private:
    bool m_busy_now;

public:
    CAI_Trader();
    ~CAI_Trader() override;

    [[nodiscard]] CAttachmentOwner* cast_attachment_owner() override { return this; }
    [[nodiscard]] CInventoryOwner* cast_inventory_owner() override { return this; }
    [[nodiscard]] CEntityAlive* cast_entity_alive() override { return this; }
    [[nodiscard]] CEntity* cast_entity() override { return this; }
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }
    [[nodiscard]] CPhysicsShellHolder* cast_physics_shell_holder() override { return this; }
    [[nodiscard]] CParticlesPlayer* cast_particles_player() override { return this; }
    [[nodiscard]] CScriptEntity* cast_script_entity() override { return this; }

    [[nodiscard]] DLL_Pure* _construct() override;
    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void net_Export(CSE_Abstract*) override;
    tmc::task<void> net_Destroy() override;

    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;
    [[nodiscard]] BOOL net_SaveRelevant() override { return inherited::net_SaveRelevant(); }

    tmc::task<void> Die(CObject* who) override;
    virtual void Think();
    void HitSignal(f32, Fvector&, CObject*, s16) override {}
    void HitImpulse(f32, Fvector&, Fvector&) override {}
    void Hit(SHit* pHDS) override;
    tmc::task<void> UpdateCL() override;

    void g_fireParams(CHudItem*, Fvector& P, Fvector& D, const bool = false) override;
    void g_WeaponBones(s32& L, s32& R1, s32& R2) override;
    [[nodiscard]] f32 ffGetFov() const override { return 150.0f; }
    [[nodiscard]] f32 ffGetRange() const override { return 30.0f; }
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    virtual void feel_touch_new(CObject* O);
    virtual void DropItemSendMessage(CObject* O);
    tmc::task<void> shedule_Update(u32 dt) override;

    [[nodiscard]] BOOL UsedAI_Locations() override;

    ///////////////////////////////////////////////////////////////////////
    [[nodiscard]] u16 PHGetSyncItemsNumber() override { return inherited ::PHGetSyncItemsNumber(); }
    [[nodiscard]] CPHSynchronize* PHGetSyncItem(u16 item) override { return inherited ::PHGetSyncItem(item); }
    void PHUnFreeze() override { return inherited ::PHUnFreeze(); }
    void PHFreeze() override { return inherited ::PHFreeze(); }
    ///////////////////////////////////////////////////////////////////////

    void reinit() override;
    void reload(gsl::czstring section) override;

    static void BoneCallback(CBoneInstance* B);
    void LookAtActor(CBoneInstance* B);

    void OnStartTrade();
    void OnStopTrade();

    // игровое имя
    [[nodiscard]] gsl::czstring Name() const override { return CInventoryOwner::Name(); }

    [[nodiscard]] bool can_attach(const CInventoryItem*) const override;
    [[nodiscard]] bool use_bolts() const override;
    void spawn_supplies() override;

    [[nodiscard]] bool bfAssignSound(CScriptEntityAction* tpEntityAction) override;
    [[nodiscard]] ALife::ERelationType tfGetRelationType(const CEntityAlive* tpEntityAlive) const override;

    [[nodiscard]] bool busy_now() const { return (m_busy_now); }

private:
    CSoundPlayer* m_sound_player;

public:
    IC CSoundPlayer& sound() const
    {
        VERIFY(m_sound_player);
        return (*m_sound_player);
    }

    [[nodiscard]] bool unlimited_ammo() override { return false; }
    [[nodiscard]] bool natural_weapon() const override { return false; }
    [[nodiscard]] bool natural_detector() const override { return false; }
    [[nodiscard]] bool AllowItemToTrade(CInventoryItem const* item, EItemPlace place) const override;

    void dialog_sound_start(LPCSTR phrase);
    void dialog_sound_stop();

private:
    CTraderAnimation* AnimMan;

public:
    CTraderAnimation& animation() { return (*AnimMan); }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CAI_Trader);

add_to_type_list(CAI_Trader);
#undef script_type_list
#define script_type_list save_type_list(CAI_Trader)
