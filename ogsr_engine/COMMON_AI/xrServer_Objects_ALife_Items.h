////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_ItemsH
#define xrServer_Objects_ALife_ItemsH

#include "xrServer_Objects_ALife.h"
#include "PHSynchronize.h"
#include "inventory_space.h"

#include "character_info_defs.h"
#include "infoportiondefs.h"

class CSE_ALifeItemAmmo;

class XR_NOVTABLE CSE_ALifeInventoryItem : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeInventoryItem);

public:
    DECLARE_SCRIPT_REGISTER_FUNCTION();

    enum
    {
        inventory_item_state_enabled = u8(1) << 0,
        inventory_item_angular_null = u8(1) << 1,
        inventory_item_linear_null = u8(1) << 2,
    };

    union mask_num_items
    {
        struct
        {
            u8 num_items : 5;
            u8 mask : 3;
        };
        u8 common;
    };

public:
    float m_fCondition;
    float m_fMass;
    u32 m_dwCost;

    float m_fDeteriorationValue;
    CSE_ALifeObject* m_self{};
    u32 m_last_update_time;

    explicit CSE_ALifeInventoryItem(LPCSTR caSection);
    ~CSE_ALifeInventoryItem() override;

    // we need this to prevent virtual inheritance :-(
    virtual CSE_Abstract* base() = 0;
    virtual const CSE_Abstract* base() const = 0;
    [[nodiscard]] virtual CSE_Abstract* init();
    virtual CSE_Abstract* cast_abstract() { return nullptr; }
    virtual CSE_ALifeInventoryItem* cast_inventory_item() { return this; }
    virtual u32 update_rate() const;
    // end of the virtual inheritance dependant code

    IC bool attached() const { return (base()->ID_Parent < 0xffff); }
    virtual bool bfUseful();

    /////////// network ///////////////
    u8 m_u8NumItems{};
    SPHNetState State;
    ///////////////////////////////////

    virtual void UPDATE_Read(NET_Packet& P);
    virtual void UPDATE_Write(NET_Packet& P);
    virtual void __STATE_Read(NET_Packet& P, u16 size);
    virtual void __STATE_Write(NET_Packet& P);
};

add_to_type_list(CSE_ALifeInventoryItem);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeInventoryItem)

class CSE_ALifeItem : public CSE_ALifeDynamicObjectVisual, public CSE_ALifeInventoryItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItem, CSE_ALifeDynamicObjectVisual, CSE_ALifeInventoryItem);

public:
    using inherited1 = CSE_ALifeDynamicObjectVisual;
    using inherited2 = CSE_ALifeInventoryItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    bool m_physics_disabled;

    explicit CSE_ALifeItem(LPCSTR caSection);
    ~CSE_ALifeItem() override;

    [[nodiscard]] CSE_Abstract* base() override;
    [[nodiscard]] const CSE_Abstract* base() const override;
    [[nodiscard]] CSE_Abstract* init() override;
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeInventoryItem* cast_inventory_item() override { return this; }
    [[nodiscard]] BOOL Net_Relevant() override;
    void OnEvent(NET_Packet& tNetPacket, u16 type, u32 time, ClientID sender) override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItem);

add_to_type_list(CSE_ALifeItem);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItem)

class CSE_ALifeItemTorch : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemTorch, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    // флаги
    enum EStats
    {
        eTorchActive = (1 << 0),
        eNightVisionActive = (1 << 1),
        eAttached = (1 << 2)
    };
    bool m_active;
    bool m_nightvision_active;
    bool m_attached;

    explicit CSE_ALifeItemTorch(LPCSTR caSection);
    ~CSE_ALifeItemTorch() override;

    [[nodiscard]] BOOL Net_Relevant() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemTorch);

add_to_type_list(CSE_ALifeItemTorch);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemTorch)

class CSE_ALifeItemAmmo : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemAmmo, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u16 a_elapsed;
    u16 m_boxSize;

    explicit CSE_ALifeItemAmmo(LPCSTR caSection);
    ~CSE_ALifeItemAmmo() override;

    [[nodiscard]] CSE_ALifeItemAmmo* cast_item_ammo() override { return this; }
    [[nodiscard]] bool __can_switch_online() const override;
    [[nodiscard]] bool __can_switch_offline() const override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemAmmo);

add_to_type_list(CSE_ALifeItemAmmo);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemAmmo)

class CSE_ALifeItemWeapon : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemWeapon, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    // возможность подключения аддонов
    enum EWeaponAddonStatus
    {
        eAddonDisabled = 0, // нельзя присоеденить
        eAddonPermanent = 1, // постоянно подключено по умолчанию
        eAddonAttachable = 2 // можно присоединять
    };

    // текущее состояние аддонов
    enum EWeaponAddonState : u8
    {
        eWeaponAddonScope = 1 << 0,
        eWeaponAddonGrenadeLauncher = 1 << 1,
        eWeaponAddonSilencer = 1 << 2,

        eWeaponAddonLaserOn = 1 << 3,
        eWeaponAddonFlashlightOn = 1 << 4,
        eWeaponMisfire = 1 << 5,

        // KRodin: TODO: эти пять свободных флагов можно использовать для хранения какой-то полезной информации, типа установлен ли на оружие лцу, фонарик и тп.
        // emaxflag = 1<<7,
    };

    EWeaponAddonStatus m_scope_status;
    EWeaponAddonStatus m_silencer_status;
    EWeaponAddonStatus m_grenade_launcher_status;

    u32 timestamp{};
    u8 wpn_flags;
    u8 wpn_state;
    u8 ammo_type;
    u16 a_current;
    u16 a_elapsed;

    LPCSTR m_caAmmoSections;
    u32 m_dwAmmoAvailable{};
    Flags8 m_addon_flags;
    u8 m_bZoom{};
    u32 m_ef_main_weapon_type;
    u32 m_ef_weapon_type;

    explicit CSE_ALifeItemWeapon(LPCSTR caSection);
    ~CSE_ALifeItemWeapon() override;

    void OnEvent(NET_Packet& P, u16 type, u32 time, ClientID sender) override;
    [[nodiscard]] u32 ef_main_weapon_type() const override;
    [[nodiscard]] u32 ef_weapon_type() const override;

    u16 get_ammo_total();
    u16 get_ammo_elapsed();
    u16 get_ammo_magsize();

    [[nodiscard]] BOOL Net_Relevant() override;

    [[nodiscard]] CSE_ALifeItemWeapon* cast_item_weapon() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemWeapon);

add_to_type_list(CSE_ALifeItemWeapon);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemWeapon)

class CSE_ALifeItemWeaponMagazined : public CSE_ALifeItemWeapon
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemWeaponMagazined, CSE_ALifeItemWeapon);

public:
    using inherited = CSE_ALifeItemWeapon;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u8 m_u8CurFireMode;

    explicit CSE_ALifeItemWeaponMagazined(LPCSTR caSection);
    ~CSE_ALifeItemWeaponMagazined() override;

    [[nodiscard]] CSE_ALifeItemWeapon* cast_item_weapon() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemWeaponMagazined);

add_to_type_list(CSE_ALifeItemWeaponMagazined);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemWeaponMagazined)

class CSE_ALifeItemWeaponMagazinedWGL : public CSE_ALifeItemWeaponMagazined
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemWeaponMagazinedWGL, CSE_ALifeItemWeaponMagazined);

public:
    using inherited = CSE_ALifeItemWeaponMagazined;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    bool m_bGrenadeMode;
    u8 ammo_type2;
    u16 a_elapsed2;

    explicit CSE_ALifeItemWeaponMagazinedWGL(LPCSTR caSection);
    ~CSE_ALifeItemWeaponMagazinedWGL() override;

    [[nodiscard]] CSE_ALifeItemWeapon* cast_item_weapon() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemWeaponMagazinedWGL);

add_to_type_list(CSE_ALifeItemWeaponMagazinedWGL);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemWeaponMagazinedWGL)

class CSE_ALifeItemWeaponShotGun : public CSE_ALifeItemWeaponMagazined
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemWeaponShotGun, CSE_ALifeItemWeaponMagazined);

public:
    using inherited = CSE_ALifeItemWeaponMagazined;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    xr_vector<u8> m_AmmoIDs;

    explicit CSE_ALifeItemWeaponShotGun(LPCSTR caSection);
    ~CSE_ALifeItemWeaponShotGun() override;

    [[nodiscard]] CSE_ALifeItemWeapon* cast_item_weapon() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemWeaponShotGun);

add_to_type_list(CSE_ALifeItemWeaponShotGun);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemWeaponShotGun)

class CSE_ALifeItemDetector : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemDetector, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u32 m_ef_detector_type;

    explicit CSE_ALifeItemDetector(LPCSTR caSection);
    ~CSE_ALifeItemDetector() override;

    [[nodiscard]] u32 ef_detector_type() const override;
    [[nodiscard]] CSE_ALifeItemDetector* cast_item_detector() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemDetector);

add_to_type_list(CSE_ALifeItemDetector);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemDetector)

class CSE_ALifeItemArtefact : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemArtefact, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    float m_fAnomalyValue;

    explicit CSE_ALifeItemArtefact(LPCSTR caSection);
    ~CSE_ALifeItemArtefact() override;

    [[nodiscard]] BOOL Net_Relevant() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemArtefact);

add_to_type_list(CSE_ALifeItemArtefact);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemArtefact)

class CSE_ALifeItemPDA : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemPDA, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u16 m_original_owner{std::numeric_limits<u16>::max()};
    shared_str m_specific_character{};
    shared_str m_info_portion{};

    explicit CSE_ALifeItemPDA(LPCSTR caSection);
    ~CSE_ALifeItemPDA() override;

    [[nodiscard]] CSE_ALifeItemPDA* cast_item_pda() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemPDA);

add_to_type_list(CSE_ALifeItemPDA);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemPDA)

class CSE_ALifeItemDocument : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemDocument, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    shared_str m_wDoc;

    explicit CSE_ALifeItemDocument(LPCSTR caSection);
    ~CSE_ALifeItemDocument() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemDocument);

add_to_type_list(CSE_ALifeItemDocument);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemDocument)

class CSE_ALifeItemGrenade : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemGrenade, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u32 m_ef_weapon_type;

    explicit CSE_ALifeItemGrenade(LPCSTR caSection);
    ~CSE_ALifeItemGrenade() override;

    [[nodiscard]] u32 ef_weapon_type() const override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemGrenade);

add_to_type_list(CSE_ALifeItemGrenade);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemGrenade)

class CSE_ALifeItemExplosive : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemExplosive, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifeItemExplosive(LPCSTR caSection);
    ~CSE_ALifeItemExplosive() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemExplosive);

add_to_type_list(CSE_ALifeItemExplosive);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemExplosive)

class CSE_ALifeItemBolt : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemBolt, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u32 m_ef_weapon_type;

    explicit CSE_ALifeItemBolt(LPCSTR caSection);
    ~CSE_ALifeItemBolt() override;

    [[nodiscard]] bool can_save() const override;
    [[nodiscard]] bool used_ai_locations() const override;
    [[nodiscard]] u32 ef_weapon_type() const override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemBolt);

add_to_type_list(CSE_ALifeItemBolt);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemBolt)

class CSE_ALifeItemCustomOutfit : public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeItemCustomOutfit, CSE_ALifeItem);

public:
    using inherited = CSE_ALifeItem;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u32 m_ef_equipment_type;

    explicit CSE_ALifeItemCustomOutfit(LPCSTR caSection);
    ~CSE_ALifeItemCustomOutfit() override;

    [[nodiscard]] u32 ef_equipment_type() const override;
    [[nodiscard]] BOOL Net_Relevant() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeItemCustomOutfit);

add_to_type_list(CSE_ALifeItemCustomOutfit);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeItemCustomOutfit)

class CSE_InventoryContainer : public CSE_InventoryBoxAbstract, public CSE_ALifeItem
{
    RTTI_DECLARE_TYPEINFO(CSE_InventoryContainer, CSE_InventoryBoxAbstract, CSE_ALifeItem);

public:
    explicit CSE_InventoryContainer(LPCSTR caSection) : CSE_ALifeItem{caSection} {}
    ~CSE_InventoryContainer() override = default;

    void add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries) override
    {
        add_offline_impl(smart_cast<CSE_ALifeDynamicObjectVisual*>(this), saved_children, update_registries);
        CSE_ALifeItem::add_offline(saved_children, update_registries);
    }

    void add_online(const bool& update_registries) override
    {
        add_online_impl(smart_cast<CSE_ALifeDynamicObjectVisual*>(this), update_registries);
        CSE_ALifeItem::add_online(update_registries);
    }
};

// KRodin: Закомментировал, попытка предотвратить повторную регистрацию cse_alife_item в луабинде.
// По идее, оно и не нужно, ведь у класса CSE_InventoryContainer нету метода ::script_register()
// add_to_type_list(CSE_InventoryContainer);
// #undef script_type_list
// #define script_type_list save_type_list(CSE_InventoryContainer)

#endif
