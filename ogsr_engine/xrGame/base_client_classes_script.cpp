////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes_script.cpp
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "base_client_classes.h"

#include "../COMMON_AI/script_camera.h"
#include "script_game_object.h"
#include "exported_classes_def.h"

// TODO: KRodin: так и хочется порезать четыре говнофункции ниже. Какой-то недоэкспорт непонятно для чего нужный.

template <>
void DLL_PureScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<DLL_Pure>("DLL_Pure", sol::no_constructor, "_construct", &DLL_Pure::_construct);
}

template <>
void ISheduledScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<ISheduled>("ISheduled", sol::no_constructor);
}

template <>
void IRenderableScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<IRenderable>("IRenderable", sol::no_constructor);
}

template <>
void ICollidableScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<ICollidable>("ICollidable", sol::no_constructor);
}

static Fvector rotation_get_dir(SRotation* R, bool v_inverse)
{
    Fvector result;
    if (v_inverse)
        result.setHP(R->yaw, -R->pitch);
    else
        result.setHP(R->yaw, R->pitch);
    return result;
}

static void rotation_set_dir(SRotation* R, const Fvector& dir, bool v_inverse)
{
    R->yaw = dir.getH();
    if (v_inverse)
        R->pitch = -dir.getP();
    else
        R->pitch = dir.getP();
    R->roll = 0;
}

static void rotation_copy(SRotation* R, SRotation* src) { memcpy(R, src, sizeof(SRotation)); }

static void rotation_init(SRotation* R, float y, float p, float r)
{
    R->pitch = p;
    R->roll = r;
    R->yaw = y;
}

template <>
void CRotationScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<SRotation>("SRotation", sol::no_constructor, sol::call_constructor, sol::constructors<SRotation(), SRotation(float, float, float)>(), "pitch",
                                &SRotation::pitch, "yaw", &SRotation::yaw, "roll", &SRotation::roll, "get_dir", &rotation_get_dir, "set_dir", &rotation_set_dir, "set",
                                sol::overload(&rotation_copy, &rotation_init));
}

template <>
void CObjectScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CGameObject>("CGameObject", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CGameObject>), "_construct", &CGameObject::_construct,
                                  "Visual", &CGameObject::Visual, "use", &CGameObject::use, "getVisible", &CGameObject::getVisible, "getEnabled", &CGameObject::getEnabled,
                                  sol::base_classes, xr::sol_bases<CGameObject>());

    lua.new_enum("global_flags",
                 // inventory_item
                 "FdropManual", CInventoryItem::EIIFlags::FdropManual, "FCanTake", CInventoryItem::EIIFlags::FCanTake, "FCanTrade", CInventoryItem::EIIFlags::FCanTrade, "Fbelt",
                 CInventoryItem::EIIFlags::Fbelt, "Fruck", CInventoryItem::EIIFlags::Fruck, "FRuckDefault", CInventoryItem::EIIFlags::FRuckDefault, "FUsingCondition",
                 CInventoryItem::EIIFlags::FUsingCondition, "FAllowSprint", CInventoryItem::EIIFlags::FAllowSprint, "Fuseful_for_NPC", CInventoryItem::EIIFlags::Fuseful_for_NPC,
                 "FInInterpolation", CInventoryItem::EIIFlags::FInInterpolation, "FInInterpolate", CInventoryItem::EIIFlags::FInInterpolate, "FIsQuestItem",
                 CInventoryItem::EIIFlags::FIsQuestItem, "FIAlwaysUntradable", CInventoryItem::EIIFlags::FIAlwaysUntradable, "FIUngroupable",
                 CInventoryItem::EIIFlags::FIUngroupable, "FIHiddenForInventory", CInventoryItem::EIIFlags::FIHiddenForInventory,

                 // se_object_flags
                 "flUseSwitches", CSE_ALifeObject::flUseSwitches, "flSwitchOnline", CSE_ALifeObject::flSwitchOnline, "flSwitchOffline", CSE_ALifeObject::flSwitchOffline,
                 "flInteractive", CSE_ALifeObject::flInteractive, "flVisibleForAI", CSE_ALifeObject::flVisibleForAI, "flUsefulForAI", CSE_ALifeObject::flUsefulForAI,
                 "flOfflineNoMove", CSE_ALifeObject::flOfflineNoMove, "flUsedAI_Locations", CSE_ALifeObject::flUsedAI_Locations, "flGroupBehaviour",
                 CSE_ALifeObject::flGroupBehaviour, "flCanSave", CSE_ALifeObject::flCanSave, "flVisibleForMap", CSE_ALifeObject::flVisibleForMap, "flUseSmartTerrains",
                 CSE_ALifeObject::flUseSmartTerrains, "flCheckForSeparator", CSE_ALifeObject::flCheckForSeparator,

                 // weapon_states,
                 "eIdle", CHudItem::EHudStates::eIdle, "eSprintStart", CHudItem::EHudStates::eSprintStart, "eSprintEnd", CHudItem::EHudStates::eSprintEnd, "eShowing",
                 CHudItem::EHudStates::eShowing, "eHiding", CHudItem::EHudStates::eHiding, "eHidden", CHudItem::EHudStates::eHidden, "eBore", CHudItem::EHudStates::eBore, "eFire",
                 CHudItem::EHudStates::eFire, "eFire2", CHudItem::EHudStates::eFire2, "eReload", CHudItem::EHudStates::eReload, "eMisfire", CHudItem::EHudStates::eMisfire,
                 "eMagEmpty", CHudItem::EHudStates::eMagEmpty, "eSwitch", CHudItem::EHudStates::eSwitch, "eDeviceSwitch", CHudItem::EHudStates::eDeviceSwitch, "eThrowStart",
                 CHudItem::EHudStates::eThrowStart, "eReady", CHudItem::EHudStates::eReady, "eThrow", CHudItem::EHudStates::eThrow, "eThrowEnd", CHudItem::EHudStates::eThrowEnd,

                 // RestrictionSpace
                 "eDefaultRestrictorTypeNone", RestrictionSpace::eDefaultRestrictorTypeNone, "eDefaultRestrictorTypeOut", RestrictionSpace::eDefaultRestrictorTypeOut,
                 "eDefaultRestrictorTypeIn", RestrictionSpace::eDefaultRestrictorTypeIn, "eRestrictorTypeNone", RestrictionSpace::eRestrictorTypeNone, "eRestrictorTypeIn",
                 RestrictionSpace::eRestrictorTypeIn, "eRestrictorTypeOut", RestrictionSpace::eRestrictorTypeOut);
}

static CCameraBase* actor_camera(u16 index)
{
    auto pA = smart_cast<CActor*>(Level().CurrentEntity());
    if (!pA)
        return nullptr;

    return pA->cam_ByIndex(index);
}

template <>
void CCameraScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CCameraBase>("CCameraBase", sol::no_constructor, "aspect", &CCameraBase::f_aspect, "direction", sol::readonly(&CCameraBase::vDirection), "fov",
                                  &CCameraBase::f_fov, "position", &CCameraBase::vPosition, "lim_yaw", &CCameraBase::lim_yaw, "lim_pitch", &CCameraBase::lim_pitch, "lim_roll",
                                  &CCameraBase::lim_roll, "yaw", &CCameraBase::yaw, "pitch", &CCameraBase::pitch, "roll", &CCameraBase::roll);

    lua.set_function("actor_camera", &actor_camera);
}

template <>
void CAnomalyDetectorScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CAnomalyDetector>("CAnomalyDetector", sol::no_constructor, "Anomaly_Detect_Radius", &CAnomalyDetector::m_radius, "Anomaly_Detect_Time_Remember",
                                       &CAnomalyDetector::m_time_to_rememeber, "Anomaly_Detect_Probability", &CAnomalyDetector::m_detect_probability, "is_active",
                                       sol::readonly(&CAnomalyDetector::m_active), "activate", &CAnomalyDetector::activate, "deactivate", &CAnomalyDetector::deactivate,
                                       "remove_all_restrictions", &CAnomalyDetector::remove_all_restrictions, "remove_restriction", &CAnomalyDetector::remove_restriction);
}

LPCSTR CPatrolPointScript::getName(CPatrolPoint* pp) { return pp->m_name.c_str(); }
void CPatrolPointScript::setName(CPatrolPoint* pp, LPCSTR str) { pp->m_name._set(str); }

void CPatrolPointScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPatrolPoint>("CPatrolPoint", sol::no_constructor, sol::call_constructor, sol::constructors<CPatrolPoint()>(), "m_position", &CPatrolPoint::m_position,
                                   "m_flags", &CPatrolPoint::m_flags, "m_level_vertex_id", &CPatrolPoint::m_level_vertex_id, "m_game_vertex_id", &CPatrolPoint::m_game_vertex_id,
                                   "m_name", sol::property(&CPatrolPointScript::getName, &CPatrolPointScript::setName), "position",
                                   sol::resolve<CPatrolPoint&(Fvector)>(&CPatrolPoint::position));
}

void CPatrolPathScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPatrolPath>("CPatrolPath", sol::no_constructor, sol::call_constructor, sol::constructors<CPatrolPath()>(), "add_point", &CPatrolPath::add_point, "point",
                                  sol::resolve<CPatrolPoint(u32), CPatrolPath>(&CPatrolPath::point), "point_raw", &CPatrolPath::point_raw, "add_vertex", &CPatrolPath::add_vertex,
                                  sol::base_classes, xr::sol_bases<CPatrolPath>());
}

namespace
{
xr_map<gsl::czstring, ITexture*> script_texture_find(gsl::czstring name)
{
    xr_map<gsl::czstring, ITexture*> ret;

    for (const auto tex : Device.m_pRender->GetResourceManager()->FindTexture(name))
        ret.try_emplace(tex->GetName(), tex); // key - texture name, value - texture object

    return ret;
}

void script_texture_load(ITexture* t, gsl::czstring name)
{
    t->Unload();
    t->Load(name);
}
} // namespace

template <>
void ITextureScript::script_register(sol::state_view& lua)
{
    lua.set_function("texture_find", &script_texture_find);
    lua.new_usertype<ITexture>("ITexture", sol::no_constructor, "load", &script_texture_load, "get_name", &ITexture::GetName);
}

template <>
void CPHCaptureScript::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPHCapture>("CPHCapture", sol::no_constructor, "e_state", sol::readonly(&CPHCapture::e_state), "capture_force", &CPHCapture::m_capture_force, "distance",
                                 &CPHCapture::m_capture_distance, "hard_mode", &CPHCapture::m_hard_mode, "pull_distance", &CPHCapture::m_pull_distance, "pull_force",
                                 &CPHCapture::m_pull_force, "time_limit", &CPHCapture::m_capture_time);

    lua.new_enum("ph_capture", "pulling", EPHCaptureState::cstPulling, "captured", EPHCaptureState::cstCaptured, "released", EPHCaptureState::cstReleased);
}
