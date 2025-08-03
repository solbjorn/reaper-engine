////////////////////////////////////////////////////////////////////////////
//	Module 		: script_net packet_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script net packet class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "script_net_packet.h"

static bool r_eof(NET_Packet* self) { return (!!self->r_eof()); }

static xr_string r_stringZ(NET_Packet* self)
{
    xr_string temp;
    self->r_stringZ(temp);
    return temp;
}

static void w_bool(NET_Packet* self, bool value) { self->w_u8(value ? 1 : 0); }

static std::tuple<u16, u32> r_begin(NET_Packet& self)
{
    u16 type;
    u32 ts = self.r_begin(type);

    return std::tuple<u16, u32>(type, ts);
}

static Fvector3 r_vec3(NET_Packet& self)
{
    Fvector3 out;
    self.r_vec3(out);
    return out;
}

static bool r_bool(NET_Packet* self) { return (!!self->r_u8()); }

static float r_angle16(NET_Packet& self)
{
    float angle;
    self.r_angle16(angle);
    return angle;
}

static float r_angle8(NET_Packet& self)
{
    float angle;
    self.r_angle8(angle);
    return angle;
}

static ClientID r_clientID(NET_Packet* self)
{
    ClientID clientID;
    self->r_clientID(clientID);
    return clientID;
}

extern u16 script_server_object_version();

template <>
void CScriptNetPacket::script_register(sol::state_view& lua)
{
    lua.set_function("script_server_object_version", &script_server_object_version);

    lua.new_usertype<ClientID>("ClientID", sol::no_constructor, sol::call_constructor, sol::constructors<ClientID(), ClientID(u32)>(), "value", &ClientID::value, "set",
                               &ClientID::set);

    lua.new_usertype<NET_Packet>(
        "net_packet", sol::no_constructor, sol::call_constructor, sol::constructors<NET_Packet()>(), "w_begin", &NET_Packet::w_begin, "w_tell", &NET_Packet::w_tell, "w_vec3",
        &NET_Packet::w_vec3, "w_float", &NET_Packet::w_float, "w_u64", &NET_Packet::w_u64, "w_s64", &NET_Packet::w_s64, "w_u32", &NET_Packet::w_u32, "w_s32", &NET_Packet::w_s32,
        "w_u16", &NET_Packet::w_u16, "w_s16", &NET_Packet::w_s16, "w_u8", &NET_Packet::w_u8, "w_s8", &NET_Packet::w_s8, "w_bool", &w_bool, "w_float_q16", &NET_Packet::w_float_q16,
        "w_float_q8", &NET_Packet::w_float_q8, "w_angle16", &NET_Packet::w_angle16, "w_angle8", &NET_Packet::w_angle8, "w_dir", &NET_Packet::w_dir, "w_sdir", &NET_Packet::w_sdir,
        "w_stringZ", sol::resolve<void(LPCSTR)>(&NET_Packet::w_stringZ), "w_matrix", &NET_Packet::w_matrix, "w_clientID", &NET_Packet::w_clientID, "r_seek", &NET_Packet::r_seek,
        "r_elapsed", &NET_Packet::r_elapsed, "r_advance", &NET_Packet::r_advance, "r_eof", &r_eof, "r_begin", &r_begin, "r_tell", &NET_Packet::r_tell, "r_vec3", &r_vec3, "r_float",
        sol::resolve<float()>(&NET_Packet::r_float), "r_u64", sol::resolve<u64()>(&NET_Packet::r_u64), "r_s64", sol::resolve<s64()>(&NET_Packet::r_s64), "r_u32",
        sol::resolve<u32()>(&NET_Packet::r_u32), "r_s32", sol::resolve<s32()>(&NET_Packet::r_s32), "r_u16", sol::resolve<u16()>(&NET_Packet::r_u16), "r_s16",
        sol::resolve<s16()>(&NET_Packet::r_s16), "r_u8", sol::resolve<u8()>(&NET_Packet::r_u8), "r_s8", sol::resolve<s8()>(&NET_Packet::r_s8), "r_bool", &r_bool, "r_float_q16",
        sol::resolve<float(float, float)>(&NET_Packet::r_float_q16), "r_float_q8", sol::resolve<float(float, float)>(&NET_Packet::r_float_q8), "r_angle16", &r_angle16, "r_angle8",
        &r_angle8, "r_dir", &NET_Packet::r_dir, "r_sdir", &NET_Packet::r_sdir, "r_stringZ", &r_stringZ, "r_matrix", &NET_Packet::r_matrix, "r_clientID", &r_clientID);
}
