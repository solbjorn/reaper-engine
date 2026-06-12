////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item.h
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_space.h"
#include "hit_immunity.h"
#include "attachable_item.h"
#include "ui\UIIconParams.h"

class CUIInventoryCellItem;

enum EHandDependence
{
    hdNone = 0,
    hd1Hand = 1,
    hd2Hand = 2
};

class CSE_Abstract;
class CGameObject;
class CFoodItem;
class CMissile;
class CHudItem;
class CWeaponAmmo;
class CWeapon;
class CPhysicsShellHolder;
class NET_Packet;
class CEatableItem;
struct SPHNetState;
class CInventoryOwner;

struct SHit;

class CInventoryItem : public CAttachableItem,
                       public CHitImmunity
#ifdef DEBUG
    ,
                       public pureRender
#endif
{
    RTTI_DECLARE_TYPEINFO(CInventoryItem, CAttachableItem, CHitImmunity
#ifdef DEBUG
                          ,
                          pureRender
#endif
    );

public:
    friend class CInventoryScript;

private:
    typedef CAttachableItem inherited;

public:
    enum EIIFlags
    {
        FdropManual = (1 << 0),
        FCanTake = (1 << 1),
        FCanTrade = (1 << 2),
        Fbelt = (1 << 3),
        Fruck = (1 << 4),
        FRuckDefault = (1 << 5),
        FUsingCondition = (1 << 6),
        FAllowSprint = (1 << 7),
        Fuseful_for_NPC = (1 << 8),
        FInInterpolation = (1 << 9),
        FInInterpolate = (1 << 10),
        FIsQuestItem = (1 << 11),
        FIAlwaysUntradable = (1 << 12),
        FIUngroupable = (1 << 13),
        FIHiddenForInventory = (1 << 14),
    };
    const u32 ClrEquipped = READ_IF_EXISTS(pSettings, r_color, "dragdrop", "color_equipped", color_argb(255, 255, 225, 0));
    const u32 ClrUntradable = READ_IF_EXISTS(pSettings, r_color, "dragdrop", "color_untradable", color_argb(255, 124, 0, 0));
    Flags16 m_flags;
    CIconParams m_icon_params;

public:
    CInventoryItem();
    ~CInventoryItem() override;

    virtual void Load(gsl::czstring section);
    void ReloadNames();

    [[nodiscard]] virtual gsl::czstring Name();
    [[nodiscard]] virtual gsl::czstring NameShort();
    shared_str ItemDescription() { return m_Description; }
    virtual void GetBriefInfo(xr_string&, xr_string&, xr_string&) {}
    virtual bool NeedBriefInfo() { return m_need_brief_info; }

    virtual tmc::task<void> OnEvent(NET_Packet& P, u16 type);

    [[nodiscard]] virtual bool Useful() const; // !!! Переопределить. (см. в Inventory.cpp)
    [[nodiscard]] virtual bool Attach(PIItem, bool) { return false; }
    [[nodiscard]] virtual bool Detach(PIItem) { return false; }
    // при детаче спаунится новая вещь при заданно названии секции
    [[nodiscard]] virtual bool Detach(gsl::czstring item_section_name, bool b_spawn_item);
    [[nodiscard]] virtual bool CanAttach(PIItem) { return false; }
    [[nodiscard]] virtual bool CanDetach(gsl::czstring) { return false; }

    [[nodiscard]] virtual EHandDependence HandDependence() const { return hd1Hand; }
    [[nodiscard]] virtual bool IsSingleHanded() const { return true; }
    virtual bool Activate(bool = false); // !!! Переопределить. (см. в Inventory.cpp)
    virtual void Deactivate(bool = false); // !!! Переопределить. (см. в Inventory.cpp)
    [[nodiscard]] virtual bool Action(EGameActions, u32) { return false; } // true если известная команда, иначе false

    virtual void OnH_B_Chield();
    void OnH_A_Chield() override;
    virtual void OnH_B_Independent(bool);
    void OnH_A_Independent() override;

    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);
    [[nodiscard]] virtual BOOL net_SaveRelevant() { return TRUE; }

#ifdef DEBUG
    virtual tmc::task<void> UpdateCL();
#else
    virtual tmc::task<void> UpdateCL() { co_return; }
#endif

    virtual void Hit(SHit* pHDS);

    BOOL GetDropManual() const { return m_flags.test(FdropManual); }
    void SetDropManual(BOOL val) { m_flags.set(FdropManual, val); }

    BOOL IsInvalid() const;

    BOOL IsQuestItem() const { return m_flags.test(FIsQuestItem); }
    [[nodiscard]] virtual u32 Cost() const { return m_cost; }
    virtual void SetCost(u32 cost) { m_cost = cost; }
    [[nodiscard]] virtual f32 Weight() const { return m_weight; }

    float m_fPsyHealthRestoreSpeed{};
    float m_fRadiationRestoreSpeed{};

    virtual float PsyHealthRestoreSpeed() const { return m_fPsyHealthRestoreSpeed; }
    [[nodiscard]] virtual f32 RadiationRestoreSpeed() const { return m_fRadiationRestoreSpeed; }

public:
    CInventory* m_pCurrentInventory{};

    u32 m_cost;
    float m_weight;
    shared_str m_Description;
    CUIInventoryCellItem* m_cell_item{};

    shared_str m_name;
    shared_str m_nameShort;
    shared_str m_nameComplex;

    EItemPlace m_eItemPlace{eItemPlaceUndefined};

    virtual void OnMoveToSlot();
    virtual void OnMoveToBelt();
    virtual void OnMoveToRuck(EItemPlace);
    virtual void OnDrop() {}
    virtual void OnBeforeDrop() {}

    int GetGridWidth() const;
    int GetGridHeight() const;
    const shared_str& GetIconName() const { return m_icon_name; }
    int GetIconIndex() const;
    int GetXPos() const;
    int GetYPos() const;

    bool GetInvShowCondition() const;

    [[nodiscard]] f32 GetCondition() const { return m_fCondition; }
    [[nodiscard]] virtual f32 GetConditionToShow() const { return GetCondition(); }

    void ChangeCondition(float fDeltaCondition);

    virtual void SetCondition(float fNewCondition)
    {
        m_fCondition = fNewCondition;
        ChangeCondition(0.0f);
    }

    u8 selected_slot;
    const xr_vector<u8>& GetSlots() { return m_slots; }
    const char* GetSlotsSect() { return m_slots_sect; }
    void SetSlot(u8 slot); // alpet: реально это SelectSlot
    virtual u8 GetSlot() const;
    u8 BaseSlot() const { return m_slots.empty() ? NO_ACTIVE_SLOT : m_slots.front(); }
    u32 GetSlotsCount() const { return m_slots.size(); }
    bool IsPlaceable(u8 min_slot, u8 max_slot);

    bool Belt() { return !!m_flags.test(Fbelt); }
    void Belt(bool on_belt) { m_flags.set(Fbelt, on_belt); }
    bool Ruck() { return !!m_flags.test(Fruck); }
    void Ruck(bool on_ruck) { m_flags.set(Fruck, on_ruck); }
    bool RuckDefault() { return !!m_flags.test(FRuckDefault); }

    [[nodiscard]] virtual bool CanTake() const { return !!m_flags.test(FCanTake); }
    [[nodiscard]] bool CanTrade() const;
    [[nodiscard]] virtual bool IsNecessaryItem(CInventoryItem* item);
    [[nodiscard]] virtual bool IsNecessaryItem(const shared_str&) { return false; }

protected:
    xr_vector<u8> m_slots;
    LPCSTR m_slots_sect;
    float m_fCondition{1.f};

    float m_fControlInertionFactor;
    shared_str m_icon_name;
    bool m_need_brief_info;

    ////////// network //////////////////////////////////////////////////
public:
    virtual void net_Export(CSE_Abstract* E);

public:
    virtual void activate_physic_shell();

    [[nodiscard]] virtual bool IsSprintAllowed() const { return !!m_flags.test(FAllowSprint); }
    [[nodiscard]] virtual f32 GetControlInertionFactor() const { return m_fControlInertionFactor; }
    [[nodiscard]] virtual bool StopSprintOnFire() { return true; }

protected:
    virtual void UpdateXForm();

public:
    virtual tmc::task<bool> net_Spawn(CSE_Abstract* DC);
    virtual tmc::task<void> net_Destroy();
    void reload(gsl::czstring section) override;
    virtual void reinit();
    virtual bool can_kill() const;
    virtual CInventoryItem* can_kill(CInventory*) const;
    virtual const CInventoryItem* can_kill(const xr_vector<const CGameObject*>&) const;
    [[nodiscard]] virtual CInventoryItem* can_make_killing(const CInventory*) const;
    [[nodiscard]] virtual bool ready_to_kill() const;
    [[nodiscard]] bool useful_for_NPC() const;

#ifdef DEBUG
    tmc::task<void> OnRender() override;
#endif

public:
    [[nodiscard]] DLL_Pure* _construct() override;

    IC CPhysicsShellHolder& object() const
    {
        VERIFY(m_object);
        return (*m_object);
    }

    virtual void on_activate_physic_shell() { R_ASSERT(0); } // sea

protected:
    float m_holder_range_modifier;
    float m_holder_fov_modifier;

public:
    virtual void modify_holder_params(f32& range, f32& fov) const;

protected:
    IC CInventoryOwner& inventory_owner() const;

private:
    CPhysicsShellHolder* m_object;

public:
    [[nodiscard]] virtual CInventoryItem* cast_inventory_item() { return this; }
    [[nodiscard]] CAttachableItem* cast_attachable_item() override { return this; }
    [[nodiscard]] virtual CPhysicsShellHolder* cast_physics_shell_holder() { return nullptr; }
    [[nodiscard]] virtual CEatableItem* cast_eatable_item() { return nullptr; }
    [[nodiscard]] virtual CWeapon* cast_weapon() { return nullptr; }
    [[nodiscard]] virtual CFoodItem* cast_food_item() { return nullptr; }
    [[nodiscard]] virtual CMissile* cast_missile() { return nullptr; }
    [[nodiscard]] virtual CHudItem* cast_hud_item() { return nullptr; }
    [[nodiscard]] virtual CWeaponAmmo* cast_weapon_ammo() { return nullptr; }
    [[nodiscard]] virtual CGameObject* cast_game_object() { return nullptr; }

private:
    u8 loaded_belt_index{std::numeric_limits<u8>::max()};
    void SetLoadedBeltIndex(u8);

public:
    u8 GetLoadedBeltIndex() { return loaded_belt_index; }
    bool m_highlight_equipped{};
    bool m_always_ungroupable{};
};
XR_SOL_BASE_CLASSES(CInventoryItem);

#include "inventory_item_inline.h"
