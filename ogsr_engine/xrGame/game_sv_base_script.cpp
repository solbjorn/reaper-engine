////////////////////////////////////////////////////////////////////////////
//	Module 		: game_sv_base_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Base server game script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xr_3da/NET_Server_Trash/net_utils.h"

#include "game_sv_base.h"
#include "xrMessages.h"
#include "ui/UIInventoryUtilities.h"
#include "xr_time.h"
#include "UI/UIGameTutorial.h"
#include "string_table.h"
#include "object_broker.h"
#include "player_hud.h"

#include <Objbase.h>

CUISequencer* g_tutorial{};
CUISequencer* g_tutorial2{};

void start_tutorial(LPCSTR name)
{
    if (g_tutorial)
    {
        VERIFY(!g_tutorial2);
        g_tutorial2 = g_tutorial;
    }

    g_tutorial = xr_new<CUISequencer>();
    g_tutorial->Start(name);
    if (g_tutorial2)
        g_tutorial->m_pStoredInputReceiver = g_tutorial2->m_pStoredInputReceiver;
}

namespace
{
void stop_tutorial()
{
    if (g_tutorial)
        g_tutorial->Stop();
}

u32 PlayHudMotion(u8 hand, LPCSTR hud_section, LPCSTR anm_name, bool bMixIn = true, float speed = 1.f, bool bOverride_item = false)
{
    return g_player_hud->script_anim_play(hand, hud_section, anm_name, bMixIn, speed, bOverride_item);
}

void StopHudMotion() { g_player_hud->script_anim_stop(); }

float MotionLength(LPCSTR hud_section, LPCSTR anm_name, float speed) { return g_player_hud->motion_length_script(hud_section, anm_name, speed); }
bool AllowHudMotion() { return g_player_hud->allow_script_anim(); }

float PlayBlendAnm(LPCSTR name, u8 part, float speed, float power, bool bLooped, bool no_restart)
{
    return g_player_hud->PlayBlendAnm(name, part, speed, power, bLooped, no_restart);
}

void StopBlendAnm(LPCSTR name, bool bForce) { g_player_hud->StopBlendAnm(name, bForce); }
void StopAllBlendAnms(bool bForce) { g_player_hud->StopAllBlendAnms(bForce); }
float SetBlendAnmTime(LPCSTR name, float time) { return g_player_hud->SetBlendAnmTime(name, time); }

LPCSTR translate_string(LPCSTR str) { return *CStringTable().translate(str); }

bool has_active_tutotial() { return !!g_tutorial; }

xr_string generate_id()
{
    GUID guid;
    CoCreateGuid(&guid);

    // 32 hex chars + 4 hyphens + null terminator
    xr_string guid_string(37, ' ');
    snprintf(guid_string.data(), guid_string.size(), "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
             guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    return guid_string;
}
} // namespace

void game_sv_GameState::script_register(sol::state_view& lua)
{
    auto game = lua.create_named_table("game");

    game.new_usertype<xrTime>("CTime", sol::no_constructor, sol::call_constructor, sol::constructors<xrTime(), xrTime(const xrTime&)>(),
                              // date_format
                              "DateToDay", sol::var(InventoryUtilities::edpDateToDay), "DateToMonth", sol::var(InventoryUtilities::edpDateToMonth), "DateToYear",
                              sol::var(InventoryUtilities::edpDateToYear),

                              // time_format
                              "TimeToHours", sol::var(InventoryUtilities::etpTimeToHours), "TimeToMinutes", sol::var(InventoryUtilities::etpTimeToMinutes), "TimeToSeconds",
                              sol::var(InventoryUtilities::etpTimeToSeconds), "TimeToMilisecs", sol::var(InventoryUtilities::etpTimeToMilisecs),

                              sol::meta_function::addition, &xrTime::operator+, sol::meta_function::subtraction, &xrTime::operator-, "diffSec", &xrTime::diffSec_script, "add",
                              sol::policies(&xrTime::add_script, sol::returns_self()), "sub", sol::policies(&xrTime::sub_script, sol::returns_self()), "setHMS",
                              sol::policies(&xrTime::setHMS, sol::returns_self()), "setHMSms", sol::policies(&xrTime::setHMSms, sol::returns_self()), "set",
                              sol::policies(&xrTime::set, sol::returns_self()), "get", &xrTime::get, "dateToString", &xrTime::dateToString, "timeToString", &xrTime::timeToString);

    game.set("time", &get_time, "get_game_time", &get_time_struct, "start_tutorial", &start_tutorial, "stop_tutorial", &stop_tutorial, "has_active_tutorial", &has_active_tutotial,
             "translate_string", &translate_string, "play_hud_motion", &PlayHudMotion, "stop_hud_motion", &StopHudMotion, "get_motion_length", &MotionLength, "hud_motion_allowed",
             &AllowHudMotion, "play_hud_anm", &PlayBlendAnm, "stop_hud_anm", &StopBlendAnm, "stop_all_hud_anms", &StopAllBlendAnms, "set_hud_anm_time", &SetBlendAnmTime,
             "generate_id", &generate_id, "StringHasUTF8", &StringHasUTF8, "StringToUTF8", &StringToUTF8, "StringFromUTF8", &StringFromUTF8);

    lua.new_enum("game_player_flags", "GAME_PLAYER_FLAG_LOCAL", GAME_PLAYER_FLAG_LOCAL);
    lua.new_enum("game_phases", "GAME_PHASE_NONE", GAME_PHASE_NONE, "GAME_PHASE_INPROGRESS", GAME_PHASE_INPROGRESS, "GAME_PHASE_PENDING", GAME_PHASE_PENDING);
    lua.new_enum("game_messages", "GAME_EVENT_PLAYER_CONNECTED", GAME_EVENT_PLAYER_CONNECTED);
}
