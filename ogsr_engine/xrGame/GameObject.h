// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\xr_3da\xr_object.h"
#include "xrServer_Space.h"
#include "alife_space.h"
#include "UsableScriptObject.h"
#include "script_binder.h"
#include "Hit.h"
#include "..\xr_3da\feel_touch.h"

class CPhysicsShell;
class CSE_Abstract;
class CPHSynchronize;
class CScriptGameObject;
class CInventoryItem;
class CEntity;
class CEntityAlive;
class CInventoryOwner;
class CActor;
class CPhysicsShellHolder;
class CParticlesPlayer;
class CCustomZone;
class IInputReceiver;
class CArtefact;
class CCustomMonster;
class CAI_Stalker;
class CScriptEntity;
class CAI_ObjectLocation;
class CWeapon;
class CExplosive;
class CHolderCustom;
class CAttachmentOwner;
class CBaseMonster;
class CSpaceRestrictor;
class CAttachableItem;
class animation_movement_controller;
class CBlend;

namespace GameObject
{
enum ECallbackType : u32;
}

struct GOCallbackInfo
{
    sol::function m_callback;
    sol::object m_object;

    [[nodiscard]] constexpr explicit operator bool() const noexcept { return !!m_callback; }

    template <typename... Args>
    constexpr void operator()(Args&&... args) const
    {
        if (!m_callback)
            return;

        if (m_object)
            m_callback(m_object, std::forward<Args>(args)...);
        else
            m_callback(std::forward<Args>(args)...);
    }
};

struct FeelTouchAddon
{
    Feel::Touch feel_touch;
    f32 radius;

    sol::function feel_touch_new_delete;
    sol::function feel_touch_contact;
    sol::object arg;

    FeelTouchAddon() = delete;

    constexpr explicit FeelTouchAddon(f32 rad, sol::function&& new_delete, sol::function&& contact, sol::object&& obj)
        : radius{rad}, feel_touch_new_delete{std::move(new_delete)}, feel_touch_contact{std::move(contact)}, arg{std::move(obj)}
    {}

    [[nodiscard]] constexpr bool equal_new_delete(const sol::function& func, const sol::object& obj) const { return feel_touch_new_delete == func && arg == obj; }
    [[nodiscard]] constexpr bool equal_contact(const sol::function& func, const sol::object& obj) const { return feel_touch_contact == func && arg == obj; }

    template <typename... Args>
    constexpr void call_new_delete(Args&&... args)
    {
        if (arg)
            feel_touch_new_delete(std::forward<Args>(args)..., arg);
        else
            feel_touch_new_delete(std::forward<Args>(args)...);
    }

    template <typename... Args>
    [[nodiscard]] constexpr bool call_contact(Args&&... args)
    {
        if (arg)
            return feel_touch_contact(std::forward<Args>(args)..., arg);
        else
            return feel_touch_contact(std::forward<Args>(args)...);
    }
};

class CGameObject : public CObject, public CUsableScriptObject, public CScriptBinder
{
    RTTI_DECLARE_TYPEINFO(CGameObject, CObject, CUsableScriptObject, CScriptBinder);

public:
    typedef CObject inherited;
    Flags32 m_server_flags;
    CAI_ObjectLocation* m_ai_location;
    ALife::_STORY_ID m_story_id;
    animation_movement_controller* m_anim_mov_ctrl{};

protected:
    // время удаления объекта
    bool m_bObjectRemoved;

public:
    CGameObject();
    ~CGameObject() override;

public:
    // functions used for avoiding most of the smart_cast
    virtual CAttachmentOwner* cast_attachment_owner() { return nullptr; }
    virtual CInventoryOwner* cast_inventory_owner() { return nullptr; }
    virtual CInventoryItem* cast_inventory_item() { return nullptr; }
    virtual CEntity* cast_entity() { return nullptr; }
    virtual CEntityAlive* cast_entity_alive() { return nullptr; }
    virtual CActor* cast_actor() { return nullptr; }
    virtual CGameObject* cast_game_object() { return this; }
    virtual CCustomZone* cast_custom_zone() { return nullptr; }
    virtual CPhysicsShellHolder* cast_physics_shell_holder() { return nullptr; }
    virtual IInputReceiver* cast_input_receiver() { return nullptr; }
    virtual CParticlesPlayer* cast_particles_player() { return nullptr; }
    virtual CArtefact* cast_artefact() { return nullptr; }
    virtual CCustomMonster* cast_custom_monster() { return nullptr; }
    virtual CAI_Stalker* cast_stalker() { return nullptr; }
    virtual CScriptEntity* cast_script_entity() { return nullptr; }
    virtual CWeapon* cast_weapon() { return nullptr; }
    virtual CExplosive* cast_explosive() { return nullptr; }
    virtual CSpaceRestrictor* cast_restrictor() { return nullptr; }
    virtual CAttachableItem* cast_attachable_item() { return nullptr; }
    virtual CHolderCustom* cast_holder_custom() { return nullptr; }
    virtual CBaseMonster* cast_base_monster() { return nullptr; }

public:
    virtual BOOL feel_touch_on_contact(CObject*) { return TRUE; }
    virtual bool use(CGameObject* who_use) { return CUsableScriptObject::use(who_use); }

public:
    CInifile* m_ini_file;
    bool m_spawned;

    // Utilities
    static void u_EventGen(NET_Packet& P, u32 type, u32 dest);
    static void u_EventSend(NET_Packet& P, u32 dwFlags = 0x0008);

    // Methods
    virtual void Load(LPCSTR section);
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void net_Relcase(CObject* O);
    virtual void UpdateCL();
    virtual void OnChangeVisual();
    // object serialization
    virtual void net_Save(NET_Packet& net_packet);
    virtual void net_Load(IReader& ireader);
    virtual BOOL net_SaveRelevant();
    void save(NET_Packet&) override;
    void load(IReader&) override;

    virtual BOOL net_Relevant() { return getLocal(); } // send messages only if active and local
    virtual void spatial_move();
    virtual BOOL Ready() { return getReady(); } // update only if active and fully initialized by/for network

    virtual void shedule_Update(u32 dt);
    virtual bool shedule_Needed();

    virtual void ForceTransform(const Fmatrix&) override {}
    virtual void ForceTransformAndDirection(const Fmatrix& m) override { ForceTransform(m); }

    void renderable_Render(u32 context_id, IRenderable* root) override;
    virtual void OnEvent(NET_Packet& P, u16 type);
    virtual void Hit(SHit*) {}
    virtual void SetHitInfo(CObject*, CObject*, s16, Fvector, Fvector) {}

    // игровое имя объекта
    virtual LPCSTR Name() const;

    // virtual void			OnH_A_Independent	();
    virtual void OnH_B_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);

    virtual bool IsVisibleForZones() { return true; }
    ///////////////////////////////////////////////////////////////////////
    virtual void DestroyObject();
    ///////////////////////////////////////////////////////////////////////

    // Position stack
    virtual SavedPosition ps_Element(u32 ID) const;

    void setup_parent_ai_locations(bool assign_position = true);
    void validate_ai_locations(bool decrement_reference = true);

    // animation_movement_controller
    virtual void create_anim_mov_ctrl(CBlend* b);
    virtual void destroy_anim_mov_ctrl();
    void update_animation_movement_controller();
    IC bool animation_movement_controlled() const { return !!animation_movement(); }
    const animation_movement_controller* animation_movement() const { return m_anim_mov_ctrl; }
    // Game-specific events

    virtual BOOL UsedAI_Locations();
    void SetUseAI_Locations(bool _use);
    BOOL TestServerFlag(u32 Flag) const;
    virtual bool can_validate_position_on_spawn() { return true; }
#ifdef DEBUG
    virtual void OnRender();
#endif

    void init();
    void reinit() override;
    void reload(LPCSTR) override;

public:
    virtual void dbg_DrawSkeleton();

    virtual const SRotation Orientation() const
    {
        SRotation rotation;
        float h, p, b;
        XFORM().getHPB(h, p, b);
        rotation.yaw = h;
        rotation.pitch = p;
        return (rotation);
    }

    virtual bool use_parent_ai_locations() const { return (true); }

public:
    typedef void visual_callback(IKinematics*);
    typedef svector<visual_callback*, 6> CALLBACK_VECTOR;
    typedef CALLBACK_VECTOR::iterator CALLBACK_VECTOR_IT;

    CALLBACK_VECTOR m_visual_callback;

public:
    void add_visual_callback(visual_callback* callback);
    void remove_visual_callback(visual_callback* callback);
    void SetKinematicsCallback(bool set);

    IC CALLBACK_VECTOR& visual_callbacks() { return (m_visual_callback); }

private:
    mutable CScriptGameObject* m_lua_game_object;
    int m_script_clsid;

public:
    CScriptGameObject* lua_game_object() const;
    int clsid() const
    {
        THROW(m_script_clsid >= 0);
        return (m_script_clsid);
    }

public:
    IC CInifile* spawn_ini() { return (m_ini_file); }

protected:
    virtual void spawn_supplies();
    virtual bool load_upgrades(CSE_Abstract*) { return false; }

public:
    IC CAI_ObjectLocation& ai_location() const
    {
        VERIFY(m_ai_location);
        return (*m_ai_location);
    }

private:
    u32 m_spawn_time{};

public:
    IC u32 spawn_time() const
    {
        VERIFY(m_spawned);
        return (m_spawn_time);
    }

    IC const ALife::_STORY_ID& story_id() const { return (m_story_id); }

public:
    virtual u32 ef_creature_type() const;
    virtual u32 ef_equipment_type() const;
    virtual u32 ef_main_weapon_type() const;
    virtual u32 ef_anomaly_type() const;
    virtual u32 ef_weapon_type() const;
    virtual u32 ef_detector_type() const;
    virtual bool natural_weapon() const { return true; }
    virtual bool natural_detector() const { return true; }
    virtual bool use_center_to_aim() const { return false; }

    xr_map<GameObject::ECallbackType, GOCallbackInfo> m_callbacks;

    const GOCallbackInfo& callback(GameObject::ECallbackType type) const;
    virtual LPCSTR visual_name(CSE_Abstract* server_entity);

    virtual void On_B_NotCurrentEntity() {}

    CSE_ALifeDynamicObject* alife_object() const; // alpet: возвращает серверный экземпляр для этого объекта

protected:
    xr_vector<FeelTouchAddon*> feel_touch_addons;
    bool feel_touch_changed, feel_touch_processing;
    void FeelTouchAddonsUpdate();
    void FeelTouchAddonsRelcase(CObject*);

public:
    void addFeelTouch(f32 radius, sol::object lua_object, sol::function new_delete, sol::function contact);
    void removeFeelTouch(sol::object lua_object, sol::function new_delete, sol::function contact);
};
XR_SOL_BASE_CLASSES(CGameObject);
