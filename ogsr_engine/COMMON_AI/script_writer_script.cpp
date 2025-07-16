#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "script_writer.h"

static void w_bool(IWriter* self, bool value) { self->w_u8(!!value); }

template <>
void CScriptWriter::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<IWriter>("writer", sol::no_constructor, "w_tell", &IWriter::tell, "w_vec3", &IWriter::w_fvector3, "w_float", &IWriter::w_float, "w_u64",
                                             &IWriter::w_u64, "w_s64", &IWriter::w_s64, "w_u32", &IWriter::w_u32, "w_s32", &IWriter::w_s32, "w_u16", &IWriter::w_u16, "w_s16",
                                             &IWriter::w_s16, "w_u8", &IWriter::w_u8, "w_s8", &IWriter::w_s8, "w_bool", &w_bool, "w_float_q16", &IWriter::w_float_q16, "w_float_q8",
                                             &IWriter::w_float_q8, "w_angle16", &IWriter::w_angle16, "w_angle8", &IWriter::w_angle8, "w_dir", &IWriter::w_dir, "w_sdir",
                                             &IWriter::w_sdir, "w_stringZ", sol::resolve<void(const char*)>(&IWriter::w_stringZ));
}
