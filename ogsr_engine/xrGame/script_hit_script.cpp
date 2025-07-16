////////////////////////////////////////////////////////////////////////////
//	Module 		: script_hit_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script hit class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_hit.h"
#include "script_game_object.h"

void CScriptHit::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CScriptHit>("hit", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptHit(), CScriptHit(const CScriptHit*)>(), "burn",
                                 sol::var(ALife::eHitTypeBurn), "shock", sol::var(ALife::eHitTypeShock), "strike", sol::var(ALife::eHitTypeStrike), "wound",
                                 sol::var(ALife::eHitTypeWound), "radiation", sol::var(ALife::eHitTypeRadiation), "telepatic", sol::var(ALife::eHitTypeTelepatic), "chemical_burn",
                                 sol::var(ALife::eHitTypeChemicalBurn), "explosion", sol::var(ALife::eHitTypeExplosion), "fire_wound", sol::var(ALife::eHitTypeFireWound), "dummy",
                                 sol::var(ALife::eHitTypeMax), "power", &CScriptHit::m_fPower, "direction", &CScriptHit::m_tDirection, "draftsman", &CScriptHit::m_tpDraftsman,
                                 "impulse", &CScriptHit::m_fImpulse, "type", &CScriptHit::m_tHitType, "bone", &CScriptHit::set_bone_name);

    // KRodin: экспортировал класс SHit в скрипты. Нужно для каллбека entity_alive_before_hit.
    // Сделано по образу и подобию движка X-Ray Extensions.
    lua.new_usertype<SHit>("SHit", sol::no_constructor, sol::call_constructor, sol::constructors<SHit()>(), "time", &SHit::Time, "packet_type", &SHit::PACKET_TYPE, "dest_id",
                           &SHit::DestID, "power", &SHit::power,

                           // Вектор
                           "dir", &SHit::dir,

                           // Сделал так, чтобы тут можно было передавать и получать скриптовые клиентские объекты.
                           "who", sol::property(&SHit::get_hit_initiator, &SHit::set_hit_initiator),

                           "who_id", &SHit::whoID, "weapon_id", &SHit::weaponID, "bone_id", &SHit::boneID,

                           // Вектор
                           "p_in_bone_space", &SHit::p_in_bone_space,

                           "impulse", &SHit::impulse, "hit_type", &SHit::hit_type, "ap", &SHit::ap, "aim_bullet", &SHit::aim_bullet, "bullet_id", &SHit::BulletID, "sender_id",
                           &SHit::SenderID,

                           // Флаг игнорирования хита. Если скриптово установить его в true, хит нанесён не будет.
                           "ignore_hit", &SHit::ignore_flag,

                           // Начальное значение хита, до обработок всякими защитами артефактов и броней.
                           "full_power", sol::readonly(&SHit::full_power));
}
