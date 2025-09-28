////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script4.cpp
//	Created 	: 14.08.2014
//  Modified 	: 15.08.2014
//	Author		: Alexander Petrov
//	Description : Script Actor (params)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "pda_space.h"
#include "memory_space.h"
#include "movement_manager_space.h"

#include "cover_point.h"

#include "script_binder_object.h"
#include "script_entity_action.h"
#include "script_game_object.h"
#include "script_hit.h"
#include "script_ini_file.h"
#include "script_monster_hit_info.h"
#include "script_sound_info.h"

#include "action_planner.h"
#include "PhysicsShell.h"

#include "script_zone.h"
#include "relation_registry.h"
#include "danger_object.h"

#include "alife_space.h"

#include "hit_immunity.h"
#include "ActorCondition.h"
#include "EntityCondition.h"
#include "holder_custom.h"

#include "ai_space_inline.h"

#include "exported_classes_def.h"
#include "script_actor.h"

#include "script_engine.h"

#include "xrServer_Objects_ALife.h"
#include "ai_object_location.h"
#include "clsid_game.h"

template <typename T>
static T* script_game_object_cast(CScriptGameObject* script_obj)
{
    CGameObject* obj = &script_obj->object();
    return smart_cast<T*>(obj);
}

static CEntityCondition* get_obj_conditions(CScriptGameObject* script_obj)
{
    CGameObject* obj = &script_obj->object();
    CActor* pA = smart_cast<CActor*>(obj);
    if (pA)
        return &pA->conditions();

    CEntity* pE = smart_cast<CEntity*>(obj);
    if (pE)
        return pE->conditions();

    return nullptr;
}

static CHitImmunity* get_obj_immunities(CScriptGameObject* script_obj)
{
    CEntityCondition* cond = get_obj_conditions(script_obj);
    if (cond)
        return smart_cast<CHitImmunity*>(cond);

    CGameObject* obj = &script_obj->object();
    CArtefact* pArt = smart_cast<CArtefact*>(obj);
    if (pArt)
        return &pArt->m_ArtefactHitImmunities;

    return nullptr;
}

static CInventory* get_obj_inventory(CScriptGameObject* script_obj)
{
    CInventoryOwner* owner = smart_cast<CInventoryOwner*>(&script_obj->object());
    if (owner)
        return owner->m_inventory;

    CHolderCustom* holder = script_obj->get_current_holder();
    if (holder)
        return holder->GetInventory();

    return nullptr;
}

CSE_ALifeDynamicObject* CScriptGameObject::alife_object() const { return object().alife_object(); }

u32 get_level_id(u32 gvid)
{
    CGameGraph& gg = ai().game_graph();
    if (gg.valid_vertex_id(gvid))
        return gg.vertex(gvid)->level_id();
    return (u32)-1; // ERROR signal
}

LPCSTR get_level_name_by_id(u32 level_id)
{
    if (level_id < 0xff)
        return ai().game_graph().header().level((GameGraph::_LEVEL_ID)level_id).name().c_str();
    else
        return "l255_invalid_level";
}

static bool get_obj_alive(CScriptGameObject* O)
{
    CGameObject* obj = &O->object();
    CEntityAlive* ent = smart_cast<CEntityAlive*>(obj);
    if (ent)
        return ent->g_Alive();
    else
        return false;
}

static bool actor_can_take(CScriptGameObject* O)
{
    CGameObject* obj = &O->object();

    if (obj->getDestroy())
        return false;

    if (!obj->getVisible())
        return false;

    CInventoryItem* pIItem = smart_cast<CInventoryItem*>(obj);
    if (!pIItem)
        return false;

    if (pIItem->object().H_Parent())
        return false;

    if (!pIItem->CanTake())
        return false;

    if (pIItem->object().CLS_ID == CLSID_OBJECT_G_RPG7 || pIItem->object().CLS_ID == CLSID_OBJECT_G_FAKE)
        return false;

    CGrenade* pGrenade = smart_cast<CGrenade*>(obj);
    if (pGrenade && !pGrenade->Useful())
        return false;

    CMissile* pMissile = smart_cast<CMissile*>(obj);
    if (pMissile && !pMissile->Useful())
        return false;

    return true;
}

static u32 obj_level_id(CScriptGameObject* O) { return get_level_id(O->object().ai_location().game_vertex_id()); }

static LPCSTR obj_level_name(CScriptGameObject* O) { return get_level_name_by_id(obj_level_id(O)); }

void CScriptGameObject::script_register3(CScriptGameObject::usertype& lua)
{
    xr::sol_set(lua,
                // alpet: export object cast
                "get_game_object", &CScriptGameObject::object, "get_alife_object", &CScriptGameObject::alife_object, "get_actor", &script_game_object_cast<CActorObject>,
                "get_anomaly", &script_game_object_cast<CCustomZone>, "get_artefact", &script_game_object_cast<CArtefact>, "get_base_monster",
                &script_game_object_cast<CBaseMonster>, "get_container", &script_game_object_cast<CInventoryContainer>, "get_custom_monster",
                &script_game_object_cast<CCustomMonster>, "get_eatable_item", &script_game_object_cast<CEatableItem>, "get_grenade", &script_game_object_cast<CGrenade>,
                "get_inventory_box", &script_game_object_cast<IInventoryBox>, "get_inventory_item", &script_game_object_cast<CInventoryItem>, "get_inventory_owner",
                &script_game_object_cast<CInventoryOwner>, "get_missile", &script_game_object_cast<CMissile>, "get_outfit", &script_game_object_cast<CCustomOutfit>,
                "get_space_restrictor", &script_game_object_cast<CSpaceRestrictor>, "get_torch", &script_game_object_cast<CTorch>, "get_weapon", &script_game_object_cast<CWeapon>,
                "get_weapon_m", &script_game_object_cast<CWeaponMagazined>, "get_weapon_magazined", &script_game_object_cast<CWeaponMagazined>, "get_weapon_mwg",
                &script_game_object_cast<CWeaponMagazinedWGrenade>, "get_weapon_gl", &script_game_object_cast<CWeaponMagazinedWGrenade>, "get_weapon_sg",
                &script_game_object_cast<CWeaponShotgun>, "get_weapon_shotgun", &script_game_object_cast<CWeaponShotgun>,

                "ph_capture_object",
                sol::overload(sol::resolve<void(CScriptGameObject*)>(&CScriptGameObject::PHCaptureObject),
                              sol::resolve<void(CScriptGameObject*, LPCSTR)>(&CScriptGameObject::PHCaptureObject),
                              sol::resolve<void(CScriptGameObject*, u16)>(&CScriptGameObject::PHCaptureObject),
                              sol::resolve<void(CScriptGameObject*, u16, LPCSTR)>(&CScriptGameObject::PHCaptureObject)),
                "ph_release_object", &CScriptGameObject::PHReleaseObject, "ph_capture", &CScriptGameObject::PHCapture, "throw_target",
                sol::overload(sol::resolve<bool(const Fvector&, CScriptGameObject*)>(&CScriptGameObject::throw_target),
                              sol::resolve<bool(const Fvector&, u32 const, CScriptGameObject*)>(&CScriptGameObject::throw_target)),

                "g_fireParams", &CScriptGameObject::g_fireParams, "can_kill_enemy", &CScriptGameObject::can_kill_enemy, "can_fire_to_enemy", &CScriptGameObject::can_fire_to_enemy,
                "register_in_combat", &CScriptGameObject::register_in_combat, "unregister_in_combat", &CScriptGameObject::unregister_in_combat, "stalker_disp_base",
                sol::overload(sol::resolve<float()>(&CScriptGameObject::stalker_disp_base), sol::resolve<void(float)>(&CScriptGameObject::stalker_disp_base),
                              sol::resolve<void(float, float)>(&CScriptGameObject::stalker_disp_base)),

                "drop_item_and_throw", &CScriptGameObject::DropItemAndThrow, "controller_psy_hit_active", &CScriptGameObject::controller_psy_hit_active, "setEnabled",
                &CScriptGameObject::setEnabled, "setVisible", &CScriptGameObject::setVisible, "actor_can_take", &actor_can_take,

                "inventory", sol::property(&get_obj_inventory), "immunities", sol::property(&get_obj_immunities), "is_alive", sol::property(&get_obj_alive), "conditions",
                sol::property(&get_obj_conditions), "level_id", sol::property(&obj_level_id), "level_name", sol::property(&obj_level_name));
}

void CScriptGameObject::script_register4(sol::state_view& lua)
{
    lua.set_function("get_actor_obj", &Actor);
    lua.set_function("get_level_id", &get_level_id);
}
