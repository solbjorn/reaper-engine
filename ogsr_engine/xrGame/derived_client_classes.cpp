////////////////////////////////////////////////////////////////////////////
//	Module 		: derived_client_classes.h
//	Created 	: 16.08.2014
//  Modified 	: 20.10.2014
//	Author		: Alexander Petrov
//	Description : XRay derived client classes script export
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "base_client_classes.h"
#include "derived_client_classes.h"
#include "HUDManager.h"
#include "exported_classes_def.h"
#include "script_game_object.h"
#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"

/* Декларация о стиле экспорта свойств и методов:
     * Свойства объектов экспортируются по возможности так, как они выглядят в файлах конфигурации (*.ltx), а не так как они названы в исходниках движка
     * Методы объектов экспортируются согласно стилю экспорта для game_object, т.е без использования прописных букв.
        Это позволяет сохранить единый стиль программирования в скриптах и отделить новые методы от исконно движковых версии 1.0006.
   Alexander Petrov
*/

// ================================ ANOMALY ZONE SCRIPT EXPORT =================== //
static Fvector get_restrictor_center(CSpaceRestrictor* SR)
{
    Fvector result;
    SR->Center(result);
    return result;
}

static u32 get_zone_state(CCustomZone* obj) { return (u32)obj->ZoneState(); }

void CAnomalyZoneScript::set_zone_state(CCustomZone* obj, u32 new_state) { obj->SwitchZoneState((CCustomZone::EZoneState)new_state); }

void CAnomalyZoneScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSpaceRestrictor>("CSpaceRestrictor", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSpaceRestrictor>), "factory",
                                       &xr::client_factory<CSpaceRestrictor>, "restrictor_center", sol::property(&get_restrictor_center), "restrictor_type",
                                       sol::property(&CSpaceRestrictor::restrictor_type), "radius", sol::property(&CSpaceRestrictor::Radius), "schedule_register",
                                       &CSpaceRestrictor::ScheduleRegister, "schedule_unregister", &CSpaceRestrictor::ScheduleUnregister, "is_scheduled",
                                       &CSpaceRestrictor::IsScheduled, "active_contact", &CSpaceRestrictor::active_contact, sol::base_classes, xr::sol_bases<CSpaceRestrictor>());

    lua.new_usertype<CCustomZone>("CustomZone", sol::no_constructor, "power", &CCustomZone::Power, "relative_power", &CCustomZone::RelativePower, "attenuation",
                                  &CCustomZone::m_fAttenuation, "effective_radius", &CCustomZone::m_fEffectiveRadius, "hit_impulse_scale", &CCustomZone::m_fHitImpulseScale,
                                  "max_power", &CCustomZone::m_fMaxPower, "state_time", &CCustomZone::m_iStateTime, "start_time", &CCustomZone::m_StartTime, "time_to_live",
                                  &CCustomZone::m_ttl, "zone_active", &CCustomZone::m_bZoneActive, "zone_state",
                                  sol::property(&get_zone_state, &CAnomalyZoneScript::set_zone_state), sol::base_classes, xr::sol_bases<CCustomZone>());
}

static void alive_entity_set_radiation(CEntityAlive* E, float value) { E->SetfRadiation(value); }

void CEntityScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CEntity>("CEntity", sol::no_constructor, sol::base_classes, xr::sol_bases<CEntity>());

    lua.new_usertype<CEntityAlive>("CEntityAlive", sol::no_constructor, "radiation", sol::property(&CEntityAlive::g_Radiation, &alive_entity_set_radiation), "condition",
                                   sol::property(&CEntityAlive::conditions), sol::base_classes, xr::sol_bases<CEntityAlive>());
}

void CEatableItemScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CEatableItem>("CEatableItem", sol::no_constructor, "eat_health", &CEatableItem::m_fHealthInfluence, "eat_power", &CEatableItem::m_fPowerInfluence,
                                   "eat_satiety", &CEatableItem::m_fSatietyInfluence, "eat_radiation", &CEatableItem::m_fRadiationInfluence, "eat_max_power",
                                   &CEatableItem::m_fMaxPowerUpInfluence, "eat_psy_health", &CEatableItem::m_fPsyHealthInfluence, "eat_thirst", &CEatableItem::m_fThirstInfluence,
                                   "wounds_heal_perc", &CEatableItem::m_fWoundsHealPerc, "eat_portions_num", &CEatableItem::m_iPortionsNum, "eat_start_portions_num",
                                   &CEatableItem::m_iStartPortionsNum, sol::base_classes, xr::sol_bases<CEatableItem>());

    lua.new_usertype<CEatableItemObject>("CEatableItemObject", sol::no_constructor, sol::base_classes, xr::sol_bases<CEatableItemObject>());
}

static void set_io_money(CInventoryOwner* IO, u32 money) { IO->set_money(money, true); }

static CScriptGameObject* item_lua_object(PIItem itm)
{
    if (itm)
    {
        CGameObject* obj = smart_cast<CGameObject*>(itm);
        if (obj)
            return obj->lua_game_object();
    }

    return nullptr;
}

static CScriptGameObject* inventory_active_item(CInventory* I) { return item_lua_object(I->ActiveItem()); }

static CScriptGameObject* inventory_selected_item(CInventory* I)
{
    CUIDialogWnd* IR = HUD().GetUI()->MainInputReceiver();
    if (!IR)
        return nullptr;

    CUIInventoryWnd* wnd = smart_cast<CUIInventoryWnd*>(IR);
    if (!wnd)
        return nullptr;

    if (wnd->GetInventory() != I)
        return nullptr;

    return item_lua_object(wnd->CurrentIItem());
}

static CScriptGameObject* get_inventory_target(CInventory* I) { return item_lua_object(I->m_pTarget); }

static LPCSTR get_item_name(CInventoryItem* I) { return I->Name(); }
static LPCSTR get_item_name_short(CInventoryItem* I) { return I->NameShort(); }

#include "string_table.h"

static void set_item_name(CInventoryItem* item, LPCSTR name) { item->m_name = CStringTable().translate(name); }
static void set_item_name_short(CInventoryItem* item, LPCSTR name) { item->m_nameShort = CStringTable().translate(name); }

static LPCSTR get_item_description(CInventoryItem* I) { return I->m_Description.c_str(); }
static void set_item_description(CInventoryItem* item, LPCSTR text) { item->m_Description = CStringTable().translate(text); }

static luabind::object get_slots(CInventoryItem* itm)
{
    // lua_State* L = ai().script_engine().lua();

    // lua_createtable(L, 0, 0);
    // int tidx = lua_gettop(L);
    // if (itm)
    //{
    //     for (u32 i = 0; i < itm->GetSlotsCount(); i++)
    //     {
    //         lua_pushinteger(L, i + 1); // key
    //         lua_pushinteger(L, itm->GetSlots()[i]);
    //         lua_settable(L, tidx);
    //     }
    // }

    auto table = luabind::newtable(ai().script_engine().lua());

    if (itm)
    {
        for (u32 i = 0; i < itm->GetSlotsCount(); i++)
        {
            table[i + 1] = itm->GetSlots()[i];
        }
    }

    return table;
}

void CInventoryScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CInventoryItem>("CInventoryItem", sol::no_constructor, "item_place", sol::readonly(&CInventoryItem::m_eItemPlace), "item_condition",
                                     &CInventoryItem::m_fCondition, "inv_weight", &CInventoryItem::m_weight, "m_flags", &CInventoryItem::m_flags, "always_ungroupable",
                                     &CInventoryItem::m_always_ungroupable, "psy_health_restore_speed", &CInventoryItem::m_fPsyHealthRestoreSpeed, "radiation_restore_speed",
                                     &CInventoryItem::m_fRadiationRestoreSpeed, "inv_name", sol::property(&get_item_name, &set_item_name), "inv_name_short",
                                     sol::property(&get_item_name_short, &set_item_name_short), "cost", sol::property(&CInventoryItem::Cost, &CInventoryItem::SetCost), "slot",
                                     sol::property(&CInventoryItem::GetSlot, &CInventoryItem::SetSlot), "slots", sol::property(&get_slots), "description",
                                     sol::property(&get_item_description, &set_item_description), sol::base_classes, xr::sol_bases<CInventoryItem>());

    lua.new_usertype<CInventoryItemObject>("CInventoryItemObject", sol::no_constructor, sol::base_classes, xr::sol_bases<CInventoryItemObject>());

    lua.new_usertype<CInventory>("CInventory", sol::no_constructor, "max_belt", sol::readonly(&CInventory::m_iMaxBelt), "max_weight", &CInventory::m_fMaxWeight, "take_dist",
                                 &CInventory::m_fTakeDist, "total_weight", sol::readonly(&CInventory::m_fTotalWeight), "active_item", sol::property(&inventory_active_item),
                                 "selected_item", sol::property(&inventory_selected_item), "target", sol::property(&get_inventory_target), "is_active_slot_blocked",
                                 &CInventory::IsActiveSlotBlocked);

    lua.new_usertype<IInventoryBox>("IInventoryBox", sol::no_constructor, "object", sol::overload(&IInventoryBox::GetObjectByIndex, &IInventoryBox::GetObjectByName),
                                    "object_count", &IInventoryBox::GetSize, "empty", &IInventoryBox::IsEmpty);
    lua.new_usertype<CInventoryBox>("CInventoryBox", sol::no_constructor, sol::base_classes, xr::sol_bases<CInventoryBox>());

    lua.new_usertype<CInventoryContainer>("CInventoryContainer", sol::no_constructor, "cost", sol::property(&CInventoryContainer::Cost), "weight",
                                          sol::property(&CInventoryContainer::Weight), "is_opened", sol::property(&CInventoryContainer::IsOpened), "open",
                                          &CInventoryContainer::open, "close", &CInventoryContainer::close, sol::base_classes, xr::sol_bases<CInventoryContainer>());

    lua.new_usertype<CInventoryOwner>("CInventoryOwner", sol::no_constructor, "inventory", sol::readonly(&CInventoryOwner::m_inventory), "talking",
                                      sol::readonly(&CInventoryOwner::m_bTalking), "allow_talk", &CInventoryOwner::m_bAllowTalk, "allow_trade", &CInventoryOwner::m_bAllowTrade,
                                      "raw_money", &CInventoryOwner::m_money, "money", sol::property(&CInventoryOwner::get_money, &set_io_money), "Name", &CInventoryOwner::Name,
                                      "SetName", &CInventoryOwner::SetName, sol::base_classes, xr::sol_bases<CInventoryOwner>());
}

void CMonsterScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CBaseMonster>("CBaseMonster", sol::no_constructor, "agressive", &CBaseMonster::m_bAggressive, "angry", &CBaseMonster::m_bAngry, "damaged",
                                   &CBaseMonster::m_bDamaged, "grownlig", &CBaseMonster::m_bGrowling, "run_turn_left", &CBaseMonster::m_bRunTurnLeft, "run_turn_right",
                                   &CBaseMonster::m_bRunTurnRight, "sleep", &CBaseMonster::m_bSleep, "state_invisible", &CBaseMonster::state_invisible, sol::base_classes,
                                   xr::sol_bases<CBaseMonster>());
}

static int curr_fire_mode(CWeaponMagazined* wpn) { return wpn->GetCurrentFireMode(); }

void COutfitScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CCustomOutfit>("CCustomOutfit", sol::no_constructor, "additional_inventory_weight", &CCustomOutfit::m_additional_weight, "additional_inventory_weight2",
                                    &CCustomOutfit::m_additional_weight2, "power_loss", &CCustomOutfit::m_fPowerLoss, "burn_protection",
                                    sol::property(&get_protection<ALife::eHitTypeBurn>, &set_protection<ALife::eHitTypeBurn>), "strike_protection",
                                    sol::property(&get_protection<ALife::eHitTypeStrike>, &set_protection<ALife::eHitTypeStrike>), "shock_protection",
                                    sol::property(&get_protection<ALife::eHitTypeShock>, &set_protection<ALife::eHitTypeShock>), "wound_protection",
                                    sol::property(&get_protection<ALife::eHitTypeWound>, &set_protection<ALife::eHitTypeWound>), "radiation_protection",
                                    sol::property(&get_protection<ALife::eHitTypeRadiation>, &set_protection<ALife::eHitTypeRadiation>), "telepatic_protection",
                                    sol::property(&get_protection<ALife::eHitTypeTelepatic>, &set_protection<ALife::eHitTypeTelepatic>), "chemical_burn_protection",
                                    sol::property(&get_protection<ALife::eHitTypeChemicalBurn>, &set_protection<ALife::eHitTypeChemicalBurn>), "explosion_protection",
                                    sol::property(&get_protection<ALife::eHitTypeExplosion>, &set_protection<ALife::eHitTypeExplosion>), "fire_wound_protection",
                                    sol::property(&get_protection<ALife::eHitTypeFireWound>, &set_protection<ALife::eHitTypeFireWound>), "wound_2_protection",
                                    sol::property(&get_protection<ALife::eHitTypeWound_2>, &set_protection<ALife::eHitTypeWound_2>), "physic_strike_protection",
                                    sol::property(&get_protection<ALife::eHitTypePhysicStrike>, &set_protection<ALife::eHitTypePhysicStrike>), sol::base_classes,
                                    xr::sol_bases<CCustomOutfit>());
}

SRotation& CWeaponScript::FireDeviation(CWeapon* wpn) { return wpn->constDeviation; }

using namespace luabind;

luabind::object CWeaponScript::get_fire_modes(CWeaponMagazined* wpn)
{
    lua_State* L = ai().script_engine().lua();
    luabind::object t = newtable(L);
    auto& vector = wpn->m_aFireModes;
    int index = 1;
    for (auto it = vector.begin(); it != vector.end(); ++it, ++index)
        t[index] = *it;

    return t;
}

void CWeaponScript::set_fire_modes(CWeaponMagazined* wpn, luabind::object const& t)
{
    if (LUA_TTABLE != t.type())
        return;
    auto& vector = wpn->m_aFireModes;
    vector.clear();
    for (auto it = t.begin(); it != t.end(); ++it)
    {
        int m = object_cast<int>(*it);
        vector.push_back(m);
    }
}

luabind::object CWeaponScript::get_hit_power(CWeapon* wpn)
{
    lua_State* L = ai().script_engine().lua();
    luabind::object t = newtable(L);
    auto& vector = wpn->fvHitPower;

    t[1] = vector.x;
    t[2] = vector.y;
    t[3] = vector.z;
    t[4] = vector.w;

    return t;
}

void CWeaponScript::set_hit_power(CWeapon* wpn, luabind::object const& t)
{
    if (LUA_TTABLE != t.type())
        return;
    auto& vector = wpn->fvHitPower;

    vector.x = object_cast<float>(t[1]);
    vector.y = object_cast<float>(t[2]);
    vector.z = object_cast<float>(t[3]);
    vector.w = object_cast<float>(t[4]);
}

static LPCSTR get_scope_name(CWeapon* I) { return I->m_sScopeName.c_str(); }

static void set_scope_name(CWeapon* item, LPCSTR text)
{
    item->m_allScopeNames.erase(std::remove(item->m_allScopeNames.begin(), item->m_allScopeNames.end(), item->m_sScopeName), item->m_allScopeNames.end());
    item->m_sScopeName = text;
    item->m_allScopeNames.push_back(item->m_sScopeName);
}

static LPCSTR get_silencer_name(CWeapon* I) { return I->m_sSilencerName.c_str(); }
static void set_silencer_name(CWeapon* item, LPCSTR text) { item->m_sSilencerName = text; }

static LPCSTR get_grenade_launcher_name(CWeapon* I) { return I->m_sGrenadeLauncherName.c_str(); }
static void set_grenade_launcher_name(CWeapon* item, LPCSTR text) { item->m_sGrenadeLauncherName = text; }

void CWeaponScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeapon>("CWeapon", sol::no_constructor,
                              // из неэкспортируемого класса CHudItemObject:
                              "state", sol::property(&CHudItemObject::GetState), "next_state", sol::property(&CHudItemObject::GetNextState),
                              // ============================================================================= //
                              // параметры отдачи влияющие на камеру
                              "cam_max_angle", &CWeapon::camMaxAngle, "cam_relax_speed", &CWeapon::camRelaxSpeed, "cam_relax_speed_ai", &CWeapon::camRelaxSpeed_AI,
                              "cam_dispersion", &CWeapon::camDispersion, "cam_dispersion_inc", &CWeapon::camDispersionInc, "cam_dispertion_frac", &CWeapon::camDispertionFrac,
                              "cam_max_angle_horz", &CWeapon::camMaxAngleHorz, "cam_step_angle_horz", &CWeapon::camStepAngleHorz,

                              "fire_dispersion_condition_factor", &CWeapon::fireDispersionConditionFactor, "misfire_probability", &CWeapon::misfireProbability,
                              "misfire_condition_k", &CWeapon::misfireConditionK, "condition_shot_dec", &CWeapon::conditionDecreasePerShot, "condition_shot_dec_silencer",
                              &CWeapon::conditionDecreasePerShotSilencer,

                              "PDM_disp_base", &CWeapon::m_fPDM_disp_base, "PDM_disp_vel_factor", &CWeapon::m_fPDM_disp_vel_factor, "PDM_disp_accel_factor",
                              &CWeapon::m_fPDM_disp_accel_factor, "PDM_crouch", &CWeapon::m_fPDM_disp_crouch, "PDM_crouch_no_acc", &CWeapon::m_fPDM_disp_crouch_no_acc,

                              "hit_type", &CWeapon::m_eHitType, "hit_impulse", &CWeapon::fHitImpulse, "bullet_speed", &CWeapon::m_fStartBulletSpeed, "fire_distance",
                              &CWeapon::fireDistance, "fire_dispersion_base", &CWeapon::fireDispersionBase, "time_to_aim", &CWeapon::m_fTimeToAim, "time_to_fire",
                              &CWeapon::fTimeToFire, "use_aim_bullet", &CWeapon::m_bUseAimBullet, "hit_power", sol::property(&get_hit_power, &set_hit_power),

                              "ammo_mag_size", &CWeapon::iMagazineSize, "scope_dynamic_zoom", &CWeapon::m_bScopeDynamicZoom, "zoom_enabled", &CWeapon::m_bZoomEnabled,
                              "zoom_factor", &CWeapon::m_fZoomFactor, "zoom_rotate_time", &CWeapon::m_fZoomRotateTime, "iron_sight_zoom_factor", &CWeapon::m_fIronSightZoomFactor,
                              "scope_zoom_factor", &CWeapon::m_fScopeZoomFactor, "zoom_rotation_factor", sol::readonly(&CWeapon::m_fZoomRotationFactor),

                              // переменные для подстройки положения аддонов из скриптов:
                              "grenade_launcher_x", &CWeapon::m_iGrenadeLauncherX, "grenade_launcher_y", &CWeapon::m_iGrenadeLauncherY, "scope_x", &CWeapon::m_iScopeX, "scope_y",
                              &CWeapon::m_iScopeY, "silencer_x", &CWeapon::m_iSilencerX, "silencer_y", &CWeapon::m_iSilencerY,

                              "scope_status", &CWeapon::m_eScopeStatus, "silencer_status", &CWeapon::m_eSilencerStatus, "grenade_launcher_status",
                              &CWeapon::m_eGrenadeLauncherStatus, "scope_name", sol::property(&get_scope_name, &set_scope_name), "silencer_name",
                              sol::property(&get_silencer_name, &set_silencer_name), "grenade_launcher_name", sol::property(&get_grenade_launcher_name, &set_grenade_launcher_name),

                              "zoom_mode", sol::readonly(&CWeapon::m_bZoomMode), "scope_inertion_factor", &CWeapon::m_fScopeInertionFactor, "scope_lense_fov_factor",
                              &CWeapon::m_fSecondVPZoomFactor, "second_vp_enabled", &CWeapon::SecondVPEnabled,

                              // отклонение при стрельбе от целика (для непристрелляного оружия).
                              "const_deviation", sol::property(&CWeaponScript::FireDeviation),

                              "ammo_elapsed", sol::property(&CWeapon::GetAmmoElapsed, &CWeapon::SetAmmoElapsed), "get_ammo_current", &CWeapon::GetAmmoCurrent, "start_fire",
                              &CWeapon::FireStart, "stop_fire", &CWeapon::FireEnd,

                              // огонь ножом - правой кнопкой? )
                              "start_fire2", &CWeapon::Fire2Start, "stop_fire2", &CWeapon::Fire2End,

                              "stop_shoothing", &CWeapon::StopShooting, "get_particles_xform", &CWeapon::get_ParticlesXFORM, "get_fire_point", &CWeapon::get_CurrentFirePoint,
                              "get_fire_point2", &CWeapon::get_CurrentFirePoint2, "get_fire_direction", &CWeapon::get_LastFD, "ready_to_kill", &CWeapon::ready_to_kill,
                              "UseScopeTexture", &CWeapon::UseScopeTexture, sol::base_classes, xr::sol_bases<CWeapon>());

    lua.new_usertype<CWeaponMagazined>(
        "CWeaponMagazined", sol::no_constructor, "shot_num", sol::readonly(&CWeaponMagazined::m_iShotNum), "queue_size", &CWeaponMagazined::m_iQueueSize, "shoot_effector_start",
        &CWeaponMagazined::m_iShootEffectorStart, "cur_fire_mode", &CWeaponMagazined::m_iCurFireMode, "fire_mode", sol::property(&curr_fire_mode), "fire_modes",
        sol::property(&get_fire_modes, &set_fire_modes), "attach_addon", &CWeaponMagazined::Attach, "detach_addon", &CWeaponMagazined::Detach, "can_attach_addon",
        &CWeaponMagazined::CanAttach, "can_detach_addon", &CWeaponMagazined::CanDetach, sol::base_classes, xr::sol_bases<CWeaponMagazined>());

    lua.new_usertype<CWeaponMagazinedWGrenade>("CWeaponMagazinedWGrenade", sol::no_constructor, "gren_mag_size", &CWeaponMagazinedWGrenade::iMagazineSize2, "switch_gl",
                                               &CWeaponMagazinedWGrenade::SwitchMode, sol::base_classes, xr::sol_bases<CWeaponMagazinedWGrenade>());
    lua.new_usertype<CMissile>("CMissile", sol::no_constructor, "destroy_time", &CMissile::m_dwDestroyTime, "destroy_time_max", &CMissile::m_dwDestroyTimeMax, sol::base_classes,
                               xr::sol_bases<CMissile>());

    lua.new_enum("addon_status", "disabled", CSE_ALifeItemWeapon::eAddonDisabled, "permanent", CSE_ALifeItemWeapon::eAddonPermanent, "attachable",
                 CSE_ALifeItemWeapon::eAddonAttachable);
    lua.new_enum("addon_flags", "scope", CSE_ALifeItemWeapon::eWeaponAddonScope, "grenade_launcher", CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, "silencer",
                 CSE_ALifeItemWeapon::eWeaponAddonSilencer, "laser", CSE_ALifeItemWeapon::eWeaponAddonLaserOn, "flashlight", CSE_ALifeItemWeapon::eWeaponAddonFlashlightOn,
                 "misfire", CSE_ALifeItemWeapon::eWeaponMisfire);
}

void CCustomMonsterScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CCustomMonster>("CCustomMonster", sol::no_constructor, "get_dest_vertex_id", &CCustomMonsterScript::GetDestVertexId, "visible_for_zones",
                                     &CCustomMonster::m_visible_for_zones, "anomaly_detector", &CCustomMonster::anomaly_detector, "curr_speed",
                                     sol::readonly(&CCustomMonster::m_fCurSpeed), sol::base_classes, xr::sol_bases<CCustomMonster>());
}
