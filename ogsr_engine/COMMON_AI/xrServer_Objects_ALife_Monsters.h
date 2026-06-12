////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_MonstersH
#define xrServer_Objects_ALife_MonstersH

#include "xrServer_Objects_ALife.h"
#include "xrServer_Objects_ALife_Items.h"
#include "character_info_defs.h"
#include "associative_vector.h"

class CALifeMonsterBrain;
class CALifeHumanBrain;
class CALifeOnlineOfflineGroupBrain;

class XR_NOVTABLE CSE_ALifeTraderAbstract : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeTraderAbstract);

public:
    DECLARE_SCRIPT_REGISTER_FUNCTION();

    enum eTraderFlags : u32
    {
        eTraderFlagInfiniteAmmo = u32(1) << 0,
    };

    u32 m_dwMoney;
    float m_fMaxItemMass;
    Flags32 m_trader_flags;

    ////////////////////////////////////////////////////
    // character profile info
    shared_str character_profile();
    void set_character_profile(shared_str);
    shared_str specific_character();
    void set_specific_character(shared_str);

    CHARACTER_COMMUNITY_INDEX m_community_index;
    CHARACTER_REPUTATION_VALUE m_reputation;
    CHARACTER_RANK_VALUE m_rank;
    xr_string m_character_name;

#ifdef XRGAME_EXPORTS
    // для работы с relation system
    u16 object_id() const;
    CHARACTER_COMMUNITY_INDEX Community() const;
    LPCSTR CommunityName() const;
    CHARACTER_RANK_VALUE Rank();
    CHARACTER_REPUTATION_VALUE Reputation();
    void SetRank(CHARACTER_RANK_VALUE val);

#endif

    shared_str m_sCharacterProfile;
    shared_str m_SpecificCharacter{};

    // буферный вектор проверенных персонажей
    xr_vector<shared_str> m_CheckedCharacters;
    xr_vector<shared_str> m_DefaultCharacters;

public:
    explicit CSE_ALifeTraderAbstract(LPCSTR caSection);
    ~CSE_ALifeTraderAbstract() override;

    // we need this to prevent virtual inheritance :-(
    virtual CSE_Abstract* base() = 0;
    virtual const CSE_Abstract* base() const = 0;
    [[nodiscard]] virtual CSE_Abstract* init();
    virtual CSE_Abstract* cast_abstract() { return nullptr; }
    virtual CSE_ALifeTraderAbstract* cast_trader_abstract() { return this; }
    // end of the virtual inheritance dependant code

#ifdef XRGAME_EXPORTS
    virtual void add_online(const bool& update_registries);
    virtual void add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries);
    void vfInitInventory();
    virtual void spawn_supplies();
#endif

    virtual void UPDATE_Read(NET_Packet& P);
    virtual void UPDATE_Write(NET_Packet& P);
    virtual void __STATE_Read(NET_Packet& P, u16 size);
    virtual void __STATE_Write(NET_Packet& P);
};

add_to_type_list(CSE_ALifeTraderAbstract);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeTraderAbstract)

class CSE_ALifeTrader : public CSE_ALifeDynamicObjectVisual, public CSE_ALifeTraderAbstract
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeTrader, CSE_ALifeDynamicObjectVisual, CSE_ALifeTraderAbstract);

public:
    using inherited1 = CSE_ALifeDynamicObjectVisual;
    using inherited2 = CSE_ALifeTraderAbstract;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifeTrader(LPCSTR caSection);
    ~CSE_ALifeTrader() override;

    [[nodiscard]] bool interactive() const override;
    [[nodiscard]] CSE_Abstract* init() override;
    [[nodiscard]] CSE_Abstract* base() override;
    [[nodiscard]] const CSE_Abstract* base() const override;
    virtual bool natural_weapon() const { return false; }
    virtual bool natural_detector() const { return false; }

#ifdef XRGAME_EXPORTS
    void spawn_supplies() override;
    void add_online(const bool& update_registries) override;
    void add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries) override;
#endif
#ifdef DEBUG
    virtual bool match_configuration() const;
#endif
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeTraderAbstract* cast_trader_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeTrader* cast_trader() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeTrader);

add_to_type_list(CSE_ALifeTrader);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeTrader)

class CSE_ALifeCustomZone : public CSE_ALifeSpaceRestrictor
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeCustomZone, CSE_ALifeSpaceRestrictor);

public:
    using inherited = CSE_ALifeSpaceRestrictor;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    ALife::EHitType m_tHitType;
    u32 m_owner_id;
    u32 m_enabled_time;
    u32 m_disabled_time;
    u32 m_start_time_shift;

    explicit CSE_ALifeCustomZone(LPCSTR caSection);
    ~CSE_ALifeCustomZone() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeCustomZone);

add_to_type_list(CSE_ALifeCustomZone);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeCustomZone)

class CSE_ALifeAnomalousZone : public CSE_ALifeCustomZone
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeAnomalousZone, CSE_ALifeCustomZone);

public:
    using inherited = CSE_ALifeCustomZone;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    CSE_ALifeItemWeapon* m_tpCurrentBestWeapon{};
    float m_offline_interactive_radius;
    u32 m_artefact_position_offset{};
    u16 m_artefact_spawn_count;

    explicit CSE_ALifeAnomalousZone(LPCSTR caSection);
    ~CSE_ALifeAnomalousZone() override;

    [[nodiscard]] CSE_Abstract* init() override;
    [[nodiscard]] CSE_Abstract* base() override;
    [[nodiscard]] const CSE_Abstract* base() const override;
    [[nodiscard]] virtual CSE_Abstract* cast_abstract() { return this; }
    [[nodiscard]] CSE_ALifeAnomalousZone* cast_anomalous_zone() override { return this; }
    [[nodiscard]] virtual u32 ef_anomaly_type() const;
    [[nodiscard]] u32 ef_weapon_type() const override;
    [[nodiscard]] virtual u32 ef_creature_type() const;
#ifdef XRGAME_EXPORTS
    void on_spawn() override;
    void spawn_artefacts();
    [[nodiscard]] virtual CSE_ALifeItemWeapon* tpfGetBestWeapon(ALife::EHitType& tHitType, float& fHitPower);
    [[nodiscard]] virtual ALife::EMeetActionType tfGetActionType(CSE_ALifeSchedulable*, int, bool);
    [[nodiscard]] virtual bool bfActive();
    [[nodiscard]] virtual CSE_ALifeDynamicObject* tpfGetBestDetector();
#endif

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeAnomalousZone);

add_to_type_list(CSE_ALifeAnomalousZone);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeAnomalousZone)

class CSE_ALifeTorridZone : public CSE_ALifeCustomZone, public CSE_Motion
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeTorridZone, CSE_ALifeCustomZone, CSE_Motion);

public:
    using inherited1 = CSE_ALifeCustomZone;
    using inherited2 = CSE_Motion;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifeTorridZone(LPCSTR caSection);
    ~CSE_ALifeTorridZone() override;

    [[nodiscard]] CSE_Motion* motion() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeTorridZone);

add_to_type_list(CSE_ALifeTorridZone);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeTorridZone)

class CSE_ALifeZoneVisual : public CSE_ALifeAnomalousZone, public CSE_Visual
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeZoneVisual, CSE_ALifeAnomalousZone, CSE_Visual);

public:
    using inherited1 = CSE_ALifeAnomalousZone;
    using inherited2 = CSE_Visual;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    shared_str attack_animation;

    explicit CSE_ALifeZoneVisual(LPCSTR caSection);
    ~CSE_ALifeZoneVisual() override;

    [[nodiscard]] CSE_Visual* visual() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeZoneVisual);

add_to_type_list(CSE_ALifeZoneVisual);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeZoneVisual)

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class CSE_ALifeCreatureAbstract : public CSE_ALifeDynamicObjectVisual
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeCreatureAbstract, CSE_ALifeDynamicObjectVisual);

public:
    using inherited = CSE_ALifeDynamicObjectVisual;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u8 s_team;
    u8 s_squad;
    u8 s_group;
    float fHealth;
    float m_fMorale;
    float m_fAccuracy;
    float m_fIntelligence;

    u32 timestamp{}; // server(game) timestamp
    u8 flags;
    float o_model; // model yaw
    SRotation o_torso; // torso in world coords
    bool m_bDeathIsProcessed;

    xr_vector<ALife::_OBJECT_ID> m_dynamic_out_restrictions;
    xr_vector<ALife::_OBJECT_ID> m_dynamic_in_restrictions;

    u32 m_ef_creature_type;
    u32 m_ef_weapon_type;
    u32 m_ef_detector_type;

    ALife::_OBJECT_ID m_killer_id;
    ALife::_TIME_ID m_game_death_time;

    explicit CSE_ALifeCreatureAbstract(LPCSTR caSection);
    ~CSE_ALifeCreatureAbstract() override;

    [[nodiscard]] virtual u8 g_team();
    [[nodiscard]] virtual u8 g_squad();
    [[nodiscard]] virtual u8 g_group();
    [[nodiscard]] f32 g_Health() const { return fHealth; }
    void s_Health(float v) { fHealth = v; }
    [[nodiscard]] bool g_Alive() const { return g_Health() > 0.0f; }
    [[nodiscard]] bool used_ai_locations() const override;
    [[nodiscard]] bool __can_switch_online() const override;
    [[nodiscard]] bool __can_switch_offline() const override;
    [[nodiscard]] virtual u32 ef_creature_type() const;
    [[nodiscard]] u32 ef_weapon_type() const override;
    [[nodiscard]] u32 ef_detector_type() const override;
    [[nodiscard]] CSE_ALifeCreatureAbstract* cast_creature_abstract() override { return this; }
#ifdef XRGAME_EXPORTS
    virtual void __on_death(CSE_Abstract* killer);
    void on_death(CSE_Abstract* killer);
    void on_spawn() override;
#endif
#ifdef DEBUG
    virtual bool match_configuration() const;
#endif

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeCreatureAbstract);

add_to_type_list(CSE_ALifeCreatureAbstract);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeCreatureAbstract)

class CSE_ALifeMonsterAbstract : public CSE_ALifeCreatureAbstract, public CSE_ALifeSchedulable
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeMonsterAbstract, CSE_ALifeCreatureAbstract, CSE_ALifeSchedulable);

public:
    using inherited1 = CSE_ALifeCreatureAbstract;
    using inherited2 = CSE_ALifeSchedulable;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    GameGraph::_GRAPH_ID m_tNextGraphID;
    GameGraph::_GRAPH_ID m_tPrevGraphID;
    float m_fGoingSpeed;
    float m_fCurrentLevelGoingSpeed;
    float m_fCurSpeed;
    float m_fDistanceFromPoint;
    float m_fDistanceToPoint;
    GameGraph::TERRAIN_VECTOR m_tpaTerrain;
    float m_fMaxHealthValue;
    float m_fRetreatThreshold;
    float m_fEyeRange;
    float m_fHitPower;
    ALife::EHitType m_tHitType;
    shared_str m_out_space_restrictors;
    shared_str m_in_space_restrictors;
    svector<float, ALife::eHitTypeMax> m_fpImmunityFactors;

    ALife::_OBJECT_ID m_smart_terrain_id;

    //---------------------------------------------------------
    // bool if monster under smart terrain and currently executes task
    // if monster on the way then (m_smart_terrain_id != 0xffff) && (!m_task_reached)
    bool m_task_reached;
    //---------------------------------------------------------

    int m_rank;

    ALife::_TIME_ID m_stay_after_death_time_interval;

public:
    ALife::_OBJECT_ID m_group_id;

public:
    explicit CSE_ALifeMonsterAbstract(LPCSTR caSection);
    ~CSE_ALifeMonsterAbstract() override;

    [[nodiscard]] f32 g_MaxHealth() const { return m_fMaxHealthValue; }
    [[nodiscard]] CSE_Abstract* init() override;
    [[nodiscard]] CSE_Abstract* base() override;
    [[nodiscard]] const CSE_Abstract* base() const override;
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeSchedulable* cast_schedulable() override { return this; }
    [[nodiscard]] CSE_ALifeMonsterAbstract* cast_monster_abstract() override { return this; }

    [[nodiscard]] CALifeMonsterBrain& brain() const
    {
        VERIFY(m_brain);
        return (*m_brain);
    }

    [[nodiscard]] virtual CALifeMonsterBrain* create_brain();
    [[nodiscard]] u32 ef_creature_type() const override;
    [[nodiscard]] u32 ef_weapon_type() const override;
    [[nodiscard]] u32 ef_detector_type() const override;

    [[nodiscard]] s32 Rank() { return m_rank; }

#ifndef XRGAME_EXPORTS
    void update() override {};
#else
    void update() override;
    [[nodiscard]] CSE_ALifeItemWeapon* tpfGetBestWeapon(ALife::EHitType& tHitType, float& fHitPower) override;
    [[nodiscard]] ALife::EMeetActionType tfGetActionType(CSE_ALifeSchedulable*, int, bool) override;
    [[nodiscard]] bool bfActive() override;
    [[nodiscard]] CSE_ALifeDynamicObject* tpfGetBestDetector() override;
    virtual void vfDetachAll(bool = false) {}
    void add_online(const bool& update_registries) override;
    void add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries) override;
    void __on_register() override;
    void __on_unregister() override;
    [[nodiscard]] Fvector draw_level_position() const override;
    [[nodiscard]] bool redundant() const override;
#endif
    [[nodiscard]] bool need_update(CSE_ALifeDynamicObject* object) override;

private:
    CALifeMonsterBrain* m_brain{};

public:
    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeMonsterAbstract);

add_to_type_list(CSE_ALifeMonsterAbstract);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeMonsterAbstract)

class CSE_ALifeCreatureActor : public CSE_ALifeCreatureAbstract, public CSE_ALifeTraderAbstract, public CSE_PHSkeleton
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeCreatureActor, CSE_ALifeCreatureAbstract, CSE_ALifeTraderAbstract, CSE_PHSkeleton);

public:
    using inherited1 = CSE_ALifeCreatureAbstract;
    using inherited2 = CSE_ALifeTraderAbstract;
    using inherited3 = CSE_PHSkeleton;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u16 mstate{};
    Fvector accel;
    Fvector velocity;
    float fRadiation;
    u8 weapon{};
    ///////////////////////////////////////////
    u16 m_u16NumItems;
    u16 m_holderID;
    SPHNetState m_AliveState{};

    // статический массив - 6 float(вектора пределов квантизации) + m_u16NumItems*(7 u8) (позиция и поворот кости)
    u8 m_BoneDataSize{};
    char m_DeadBodyData[1024]{};
    ///////////////////////////////////////////

    explicit CSE_ALifeCreatureActor(LPCSTR caSection);
    ~CSE_ALifeCreatureActor() override;

    [[nodiscard]] CSE_Abstract* base() override;
    [[nodiscard]] const CSE_Abstract* base() const override;
    [[nodiscard]] CSE_Abstract* init() override;
    void load(NET_Packet& tNetPacket) override;
    [[nodiscard]] bool can_save() const override { return true; }
    [[nodiscard]] virtual bool natural_weapon() const { return false; }
    [[nodiscard]] virtual bool natural_detector() const { return false; }
#ifdef XRGAME_EXPORTS
    void spawn_supplies() override;
    void add_online(const bool& update_registries) override;
    void add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries) override;
#endif
#ifdef DEBUG
    virtual bool match_configuration() const;
#endif
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeTraderAbstract* cast_trader_abstract() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeCreatureActor);

add_to_type_list(CSE_ALifeCreatureActor);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeCreatureActor)

class CSE_ALifeCreatureCrow : public CSE_ALifeCreatureAbstract
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeCreatureCrow, CSE_ALifeCreatureAbstract);

public:
    using inherited = CSE_ALifeCreatureAbstract;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifeCreatureCrow(LPCSTR caSection);
    ~CSE_ALifeCreatureCrow() override;

    [[nodiscard]] bool used_ai_locations() const override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeCreatureCrow);

add_to_type_list(CSE_ALifeCreatureCrow);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeCreatureCrow)

class CSE_ALifeCreaturePhantom : public CSE_ALifeCreatureAbstract
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeCreaturePhantom, CSE_ALifeCreatureAbstract);

public:
    using inherited = CSE_ALifeCreatureAbstract;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifeCreaturePhantom(LPCSTR caSection);
    ~CSE_ALifeCreaturePhantom() override;

    [[nodiscard]] bool used_ai_locations() const override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeCreaturePhantom);

add_to_type_list(CSE_ALifeCreaturePhantom);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeCreaturePhantom)

class CSE_ALifeMonsterZombie : public CSE_ALifeMonsterAbstract
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeMonsterZombie, CSE_ALifeMonsterAbstract);

public:
    using inherited = CSE_ALifeMonsterAbstract;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    // Personal characteristics:
    float fEyeFov;
    float fEyeRange;
    float fMinSpeed;
    float fMaxSpeed;
    float fAttackSpeed;
    float fMaxPursuitRadius;
    float fMaxHomeRadius;
    // attack
    float fHitPower;
    u16 u16HitInterval;
    float fAttackDistance;
    float fAttackAngle;

    explicit CSE_ALifeMonsterZombie(LPCSTR caSection); // constructor for variable initialization
    ~CSE_ALifeMonsterZombie() override;

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeMonsterZombie);

add_to_type_list(CSE_ALifeMonsterZombie);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeMonsterZombie)

class CSE_ALifeMonsterBase : public CSE_ALifeMonsterAbstract, public CSE_PHSkeleton
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeMonsterBase, CSE_ALifeMonsterAbstract, CSE_PHSkeleton);

public:
    using inherited1 = CSE_ALifeMonsterAbstract;
    using inherited2 = CSE_PHSkeleton;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    u16 m_spec_object_id;

    explicit CSE_ALifeMonsterBase(LPCSTR caSection); // constructor for variable initialization
    ~CSE_ALifeMonsterBase() override;

    void load(NET_Packet& tNetPacket) override;
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    void spawn_supplies(gsl::czstring) override {}
    void spawn_supplies() override {}
#ifdef XRGAME_EXPORTS
    void on_spawn() override;
    void add_online(const bool& update_registries) override;
    void add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries) override;
#endif // XRGAME_EXPORTS

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeMonsterBase);

add_to_type_list(CSE_ALifeMonsterBase);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeMonsterBase)

class CSE_ALifePsyDogPhantom : public CSE_ALifeMonsterBase
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifePsyDogPhantom, CSE_ALifeMonsterBase);

public:
    using inherited = CSE_ALifeMonsterBase;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifePsyDogPhantom(LPCSTR caSection); // constructor for variable initialization
    ~CSE_ALifePsyDogPhantom() override;

    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    [[nodiscard]] bool bfActive() override { return false; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifePsyDogPhantom);

add_to_type_list(CSE_ALifePsyDogPhantom);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifePsyDogPhantom)

//-------------------------------

class CSE_ALifeHumanAbstract : public CSE_ALifeTraderAbstract, public CSE_ALifeMonsterAbstract
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeHumanAbstract, CSE_ALifeTraderAbstract, CSE_ALifeMonsterAbstract);

public:
    using inherited1 = CSE_ALifeTraderAbstract;
    using inherited2 = CSE_ALifeMonsterAbstract;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifeHumanAbstract(LPCSTR caSection);
    ~CSE_ALifeHumanAbstract() override;

    [[nodiscard]] CSE_Abstract* init() override;
    [[nodiscard]] CSE_Abstract* base() override;
    [[nodiscard]] const CSE_Abstract* base() const override;
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeTraderAbstract* cast_trader_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeHumanAbstract* cast_human_abstract() override { return this; }
    [[nodiscard]] bool natural_weapon() const override { return false; }
    [[nodiscard]] bool natural_detector() const override { return false; }

    [[nodiscard]] CALifeHumanBrain& brain() const
    {
        VERIFY(m_brain);
        return (*m_brain);
    }

    [[nodiscard]] CALifeMonsterBrain* create_brain() override;

#ifdef XRGAME_EXPORTS
    void update() override;
    [[nodiscard]] CSE_ALifeItemWeapon* tpfGetBestWeapon(ALife::EHitType&, float&) override;
    [[nodiscard]] bool bfPerformAttack() override;
    void vfUpdateWeaponAmmo() override;
    void vfProcessItems() override;
    void vfAttachItems(ALife::ETakeType = ALife::eTakeTypeAll) override;
    [[nodiscard]] ALife::EMeetActionType tfGetActionType(CSE_ALifeSchedulable* tpALifeSchedulable, int iGroupIndex, bool bMutualDetection) override;
    [[nodiscard]] CSE_ALifeDynamicObject* tpfGetBestDetector() override;
    void vfDetachAll(bool bFictitious = false) override;
    void spawn_supplies() override;
    void __on_register() override;
    void __on_unregister() override;
    void add_online(const bool& update_registries) override;
    void add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries) override;
#endif

private:
    CALifeHumanBrain* m_brain{};

public:
    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeHumanAbstract);

add_to_type_list(CSE_ALifeHumanAbstract);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeHumanAbstract)

class CSE_ALifeHumanStalker : public CSE_ALifeHumanAbstract, public CSE_PHSkeleton
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeHumanStalker, CSE_ALifeHumanAbstract, CSE_PHSkeleton);

public:
    using inherited1 = CSE_ALifeHumanAbstract;
    using inherited2 = CSE_PHSkeleton;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    shared_str m_start_dialog;

    explicit CSE_ALifeHumanStalker(LPCSTR caSection);
    ~CSE_ALifeHumanStalker() override;

    void load(NET_Packet& tNetPacket) override;
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeHumanStalker);

add_to_type_list(CSE_ALifeHumanStalker);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeHumanStalker)

class CSE_ALifeOnlineOfflineGroup : public CSE_ALifeDynamicObject, public CSE_ALifeSchedulable
{
    RTTI_DECLARE_TYPEINFO(CSE_ALifeOnlineOfflineGroup, CSE_ALifeDynamicObject, CSE_ALifeSchedulable);

public:
    using inherited1 = CSE_ALifeDynamicObject;
    using inherited2 = CSE_ALifeSchedulable;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    explicit CSE_ALifeOnlineOfflineGroup(LPCSTR caSection);
    ~CSE_ALifeOnlineOfflineGroup() override;

    [[nodiscard]] CSE_Abstract* base() override;
    [[nodiscard]] const CSE_Abstract* base() const override;
    [[nodiscard]] CSE_Abstract* init() override;
    [[nodiscard]] CSE_Abstract* cast_abstract() override { return this; }
    [[nodiscard]] CSE_ALifeSchedulable* cast_schedulable() override { return this; }
    [[nodiscard]] CSE_ALifeOnlineOfflineGroup* cast_online_offline_group() override { return this; }

    typedef CSE_ALifeHumanStalker MEMBER;
    typedef associative_vector<ALife::_OBJECT_ID, MEMBER*> MEMBERS;

private:
    MEMBERS m_members;

#ifdef XRGAME_EXPORTS
private:
    CALifeOnlineOfflineGroupBrain* m_brain{};

public:
    [[nodiscard]] CALifeOnlineOfflineGroupBrain& brain() const;

public:
    [[nodiscard]] CSE_ALifeItemWeapon* tpfGetBestWeapon(ALife::EHitType&, float&) override;
    [[nodiscard]] ALife::EMeetActionType tfGetActionType(CSE_ALifeSchedulable*, int, bool) override;
    [[nodiscard]] bool bfActive() override;
    [[nodiscard]] CSE_ALifeDynamicObject* tpfGetBestDetector() override;
    void update() override;
    void register_member(ALife::_OBJECT_ID member_id);
    void unregister_member(ALife::_OBJECT_ID member_id);
    void notify_on_member_death(MEMBER* member);
    [[nodiscard]] MEMBER* member(ALife::_OBJECT_ID member_id, bool no_assert = false);
    void __on_before_register() override;
    void on_after_game_load();
    [[nodiscard]] bool synchronize_location() override;
    void try_switch_online() override;
    void try_switch_offline() override;
    void switch_online() override;
    void switch_offline() override;
    [[nodiscard]] bool redundant() const override;
    [[nodiscard]] ALife::_OBJECT_ID commander_id();
#else
    void update() override {}
#endif

    void UPDATE_Read(NET_Packet& P) override;
    void UPDATE_Write(NET_Packet& P) override;
    void __STATE_Read(NET_Packet& P, u16 size) override;
    void __STATE_Write(NET_Packet& P) override;
};
XR_SOL_BASE_CLASSES(CSE_ALifeOnlineOfflineGroup);

add_to_type_list(CSE_ALifeOnlineOfflineGroup);
#undef script_type_list
#define script_type_list save_type_list(CSE_ALifeOnlineOfflineGroup)

void setup_location_types(GameGraph::TERRAIN_VECTOR& m_vertex_types, CInifile* ini, LPCSTR string);

#endif
