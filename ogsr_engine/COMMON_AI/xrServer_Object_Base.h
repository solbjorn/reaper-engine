////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Object_Base.h
//	Created 	: 19.09.2002
//  Modified 	: 16.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server base object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_Abstract.h"
#include "object_interfaces.h"
#include "script_value_container.h"
#include "alife_space.h"
#include "../xr_3da/NET_Server_Trash/client_id.h"

class NET_Packet;
class xrClientData;
class CSE_ALifeGroupAbstract;
class CSE_ALifeSchedulable;
class CSE_ALifeInventoryItem;
class CSE_ALifeTraderAbstract;
class CSE_ALifeObject;
class CSE_ALifeDynamicObject;
class CSE_ALifeItemAmmo;
class CSE_ALifeItemWeapon;
class CSE_ALifeItemDetector;
class CSE_ALifeMonsterAbstract;
class CSE_ALifeHumanAbstract;
class CSE_ALifeAnomalousZone;
class CSE_ALifeTrader;
class CSE_ALifeCreatureAbstract;
class CSE_ALifeSmartZone;
class CSE_ALifeOnlineOfflineGroup;
class CSE_ALifeItemPDA;

class CPureServerObject : public IPureServerObject
{
    RTTI_DECLARE_TYPEINFO(CPureServerObject, IPureServerObject);

public:
    using inherited = IPureServerObject;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    ~CPureServerObject() override = default;

    void load(IReader& tFileStream) override;
    void save(IWriter& tMemoryStream) override;
    virtual void load(NET_Packet& tNetPacket);
    virtual void save(NET_Packet& tNetPacket);
};

class CSE_Abstract : public ISE_Abstract, public CPureServerObject, public CScriptValueContainer
{
    RTTI_DECLARE_TYPEINFO(CSE_Abstract, ISE_Abstract, CPureServerObject, CScriptValueContainer);

public:
    using inherited1 = ISE_Abstract;
    using inherited2 = CPureServerObject;
    using inherited3 = CScriptValueContainer;

    DECLARE_SCRIPT_REGISTER_FUNCTION();

    enum ESpawnFlags
    {
        flSpawnEnabled = u32(1 << 0),
        flSpawnOnSurgeOnly = u32(1 << 1),
        flSpawnSingleItemOnly = u32(1 << 2),
        flSpawnIfDestroyedOnly = u32(1 << 3),
        flSpawnInfiniteCount = u32(1 << 4),
        flSpawnDestroyOnSpawn = u32(1 << 5),
    };

private:
    LPSTR s_name_replace{};

public:
    BOOL net_Ready;
    BOOL net_Processed{}; // Internal flag for connectivity-graph

    u16 m_wVersion;
    u16 m_script_version;
    u16 RespawnTime;

    u16 ID; // internal ID
    u16 ID_Parent; // internal ParentID, 0xffff means no parent
    u16 ID_Phantom; // internal PhantomID, 0xffff means no phantom
    xrClientData* owner{};

    // spawn data
    shared_str s_name;
    u8 s_gameid;
    u8 s_RP;
    Flags16 s_flags; // state flags
    xr_vector<u16> children;

    // update data
    Fvector o_Position;
    Fvector o_Angle;
    CLASS_ID m_tClassID;
    int m_script_clsid;
    shared_str m_ini_string;
    CInifile* m_ini_file{};

    // for ALife control
    bool m_bALifeControl;
    ALife::_SPAWN_ID m_tSpawnID{};

    // ALife spawn params
    // obsolete, just because we hope to uncomment all this stuff
    Flags32 m_spawn_flags;

    // client object custom data serialization
    xr_vector<u8> client_data;

protected:
    enum class server_ops : s32
    {
        // CSE_Abstract
        STATE_READ,
        STATE_WRITE,

        // CSE_ALifeObject
        CAN_SWITCH_OFFLINE,
        CAN_SWITCH_ONLINE,
        KEEP_SAVED_DATA_ANYWAY,

        // CSE_ALifeDynamicObject
        ON_BEFORE_REGISTER,
        ON_REGISTER,
        ON_UNREGISTER,

        // CSE_ALifeCreatureAbstract
        ON_DEATH,

        // CSE_ALifeSmartZone
        ENABLED,
        REGISTER_NPC,
        SUITABLE,
        TASK,
        UNREGISTER_NPC,
        UPDATE,
    };

    sol::object priv;
    xr_map<server_ops, sol::function> ops;

public:
    void load(NET_Packet& tNetPacket) override;

    //////////////////////////////////////////////////////////////////////////

    explicit CSE_Abstract(LPCSTR caSection);
    ~CSE_Abstract() override;

    virtual void OnEvent(NET_Packet&, u16, u32, ClientID) {}
    [[nodiscard]] virtual BOOL Net_Relevant() { return TRUE; }
    //
    void STATE_Write(NET_Packet& tNetPacket);
    void Spawn_Write(NET_Packet& tNetPacket, BOOL bLocal) override;
    void STATE_Read(NET_Packet& tNetPacket, u16 size);
    [[nodiscard]] BOOL Spawn_Read(NET_Packet& tNetPacket) override;
    [[nodiscard]] gsl::czstring name() const override;
    [[nodiscard]] gsl::czstring name_replace() const override;
    void set_name(gsl::czstring s) override { s_name._set(s); }

    void set_name_replace(gsl::czstring s) override
    {
        xr_free(s_name_replace);
        s_name_replace = xr_strdup(s);
    }

    [[nodiscard]] Fvector3& position() override;
    [[nodiscard]] Fvector3& angle() override;
    [[nodiscard]] Flags16& flags() override;
    [[nodiscard]] CSE_Visual* visual() override;
    [[nodiscard]] ISE_Shape* shape() override;
    [[nodiscard]] CSE_Motion* motion() override;
    [[nodiscard]] bool validate() override;
    void on_render(CDUInterface*, ISE_AbstractLEOwner*, bool, const Fmatrix&, s32, bool) override {}
    //

    IC const Fvector& Position() const { return o_Position; }
    // we need this to prevent virtual inheritance :-(
    [[nodiscard]] virtual CSE_Abstract* base();
    [[nodiscard]] virtual const CSE_Abstract* base() const;
    [[nodiscard]] virtual CSE_Abstract* init();
    [[nodiscard]] virtual bool match_configuration() const { return true; }
    // end of the virtual inheritance dependant code

    IC int script_clsid() const
    {
        VERIFY(m_script_clsid >= 0);
        return (m_script_clsid);
    }

    CInifile& spawn_ini();

    // for smart cast
    [[nodiscard]] virtual CSE_ALifeGroupAbstract* cast_group_abstract() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeSchedulable* cast_schedulable() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeInventoryItem* cast_inventory_item() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeTraderAbstract* cast_trader_abstract() { return nullptr; }

    [[nodiscard]] virtual CSE_ALifeObject* cast_alife_object() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeDynamicObject* cast_alife_dynamic_object() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeItemAmmo* cast_item_ammo() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeItemWeapon* cast_item_weapon() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeItemDetector* cast_item_detector() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeMonsterAbstract* cast_monster_abstract() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeHumanAbstract* cast_human_abstract() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeAnomalousZone* cast_anomalous_zone() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeTrader* cast_trader() { return nullptr; }

    [[nodiscard]] virtual CSE_ALifeCreatureAbstract* cast_creature_abstract() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeSmartZone* cast_smart_zone() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeOnlineOfflineGroup* cast_online_offline_group() { return nullptr; }
    [[nodiscard]] virtual CSE_ALifeItemPDA* cast_item_pda() { return nullptr; }
};

add_to_type_list(CSE_Abstract);
#undef script_type_list
#define script_type_list save_type_list(CSE_Abstract)

namespace xr
{
template <typename T>
[[nodiscard]] constexpr std::unique_ptr<CSE_Abstract> server_factory(std::unique_ptr<T>& self)
{
    return absl::WrapUnique(static_cast<CSE_Abstract*>(self.release()));
}
} // namespace xr

u16 script_server_object_version();
