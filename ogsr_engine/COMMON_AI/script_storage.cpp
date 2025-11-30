////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_storage.h"

#include "script_engine.h"
#include "ai_space.h"

#include <format>

namespace
{
// KRodin: this не убирать ни в коем случае! Он нужен для того, чтобы классы регистрировались внутри модуля в котором находятся, а не в _G
constexpr absl::string_view FILE_HEADER{
    "\
local function script_name() \
return '{0}' \
end; \
local this; \
module('{0}', package.seeall, function(m) this = m end); \
{1}"};
} // namespace

//*********************************************************************************************
void CScriptStorage::dump_state()
{
    static bool reentrantGuard = false;
    if (reentrantGuard)
        return;
    reentrantGuard = true;

    auto L = lua().lua_state();
    lua_Debug l_tDebugInfo;

    for (int i = 0; lua_getstack(L, i, &l_tDebugInfo); ++i)
    {
        lua_getinfo(L, "nSlu", &l_tDebugInfo);
        Msg("\tLocals: ");
        const char* name = nullptr;
        int VarID = 1;
        while ((name = lua_getlocal(L, &l_tDebugInfo, VarID++)) != nullptr)
        {
            LogVariable(L, name, 1);

            lua_pop(L, 1); /* remove variable value */
        }
        m_dumpedObjList.clear();
        Msg("\tEnd");
    }

    reentrantGuard = false;
}

void CScriptStorage::LogTable(lua_State* l, LPCSTR S, int level)
{
    if (!lua_istable(l, -1))
        return;

    lua_pushnil(l); /* first key */
    while (lua_next(l, -2) != 0)
    {
        char sname[256];
        char sFullName[256];
        xr_sprintf(sname, "%s", lua_tostring(l, -2));
        xr_sprintf(sFullName, "%s.%s", S, sname);
        LogVariable(l, sFullName, level + 1);

        lua_pop(l, 1); /* removes `value'; keeps `key' for next iteration */
    }
}

void CScriptStorage::LogVariable(lua_State* l, const char* name, int level)
{
    const char* type;
    int ntype = lua_type(l, -1);
    type = lua_typename(l, ntype);

    auto tabBuffer = std::make_unique<char[]>(level + 1);
    memset(tabBuffer.get(), '\t', level);

    char value[128];

    switch (ntype)
    {
    case LUA_TFUNCTION: xr_strcpy(value, "[[function]]"); break;
    case LUA_TTHREAD: xr_strcpy(value, "[[thread]]"); break;
    case LUA_TNUMBER: xr_sprintf(value, "%f", lua_tonumber(l, -1)); break;
    case LUA_TBOOLEAN: xr_sprintf(value, "%s", lua_toboolean(l, -1) ? "true" : "false"); break;
    case LUA_TSTRING: xr_sprintf(value, "%.127s", lua_tostring(l, -1)); break;
    case LUA_TTABLE:
        if (level <= 3)
        {
            Msg("%s Table: %s", tabBuffer.get(), name);
            LogTable(l, name, level + 1);
            return;
        }
        else
        {
            xr_sprintf(value, "[...]");
        }
        break;
    case LUA_TUSERDATA: {
        auto obj = lua_touserdata(l, -1);

        // Skip already dumped object
        if (m_dumpedObjList.find(obj) != m_dumpedObjList.end())
            return;
        m_dumpedObjList.insert(obj);

        xr_strcpy(value, sol::associated_type_name(l, -1, sol::type::userdata).c_str());
    }
    break;
    default: xr_strcpy(value, "[not available]"); break;
    }

    Msg("%s %s %s : %s", tabBuffer.get(), type, name, value);
}

//*********************************************************************************************

namespace
{
void lua_panic(s32 code)
{
    auto L = ai().script_engine().lua().lua_state();
    xr_string data;
    size_t len;

    gsl::czstring desc = lua_tolstring(L, -1, &len);
    if (desc != nullptr)
    {
        data.assign(desc, len);
        lua_pop(L, 1);

        desc = data.c_str();
    }
    else
    {
        desc = "An unexpected error occurred and panic has been invoked";
    }

    xr_string expr = "script " + sol::to_string(sol::call_status{code}) + " error";
    Debug.backend(expr.c_str(), desc, nullptr, nullptr, DEBUG_INFO);
}

void ScriptCrashHandler(bool dump_lua_locals)
{
    if (!Device.OnMainThread())
        return;

    try
    {
        Msg("***************************[ScriptCrashHandler]**********************************");
        ai().script_engine().print_stack();
        if (dump_lua_locals)
            ai().script_engine().dump_state();
        Msg("*********************************************************************************");
    }
    catch (...)
    {
        Msg("Can't dump script call stack - Engine corrupted");
    }
}
} // namespace

CScriptStorage::~CScriptStorage()
{
    R_ASSERT(Debug.set_lua_trace(nullptr) == ScriptCrashHandler);
    R_ASSERT(Debug.set_lua_panic(nullptr) == lua_panic);
}

void CScriptStorage::reinit()
{
    m_virtual_machine.emplace();

    std::ignore = Debug.set_lua_panic(lua_panic);
    std::ignore = Debug.set_lua_trace(ScriptCrashHandler);
}

void CScriptStorage::close() { m_virtual_machine.reset(); }

void CScriptStorage::print_stack()
{
    auto L = lua().lua_state();
    luaL_traceback(L, L, nullptr, 0);
    size_t len;

    gsl::czstring stack = lua_tolstring(L, -1, &len);
    if (stack == nullptr)
        return;

    xr_string data(stack, len);
    lua_pop(L, 1);

    Log(data);
}

#ifdef DEBUG
void CScriptStorage::script_log(ScriptStorage::ELuaMessageType tLuaMessageType, const char* caFormat, ...) // Используется в очень многих местах //Очень много пишет в лог.
{
    va_list marker;
    va_start(marker, caFormat);
    //
    LPCSTR S = "";
    LPSTR S1;
    string4096 S2;
    switch (tLuaMessageType)
    {
    case ScriptStorage::eLuaMessageTypeInfo: S = "[LUA INFO]"; break;
    case ScriptStorage::eLuaMessageTypeError: S = "[LUA ERROR]"; break;
    case ScriptStorage::eLuaMessageTypeMessage: S = "[LUA MESSAGE]"; break;
    case ScriptStorage::eLuaMessageTypeHookCall: S = "[LUA HOOK_CALL]"; break;
    case ScriptStorage::eLuaMessageTypeHookReturn: S = "[LUA HOOK_RETURN]"; break;
    case ScriptStorage::eLuaMessageTypeHookLine: S = "[LUA HOOK_LINE]"; break;
    case ScriptStorage::eLuaMessageTypeHookCount: S = "[LUA HOOK_COUNT]"; break;
    case ScriptStorage::eLuaMessageTypeHookTailReturn: S = "[LUA HOOK_TAIL_RETURN]"; break;
    default: NODEFAULT;
    }
    xr_strcpy(S2, S);
    S1 = S2 + xr_strlen(S);
    vsprintf(S1, caFormat, marker);
    Msg("-----------------------------------------");
    Msg("[script_log] %s", S2);
    print_stack();
    Msg("-----------------------------------------");
    va_end(marker);
}
#endif

bool CScriptStorage::do_file(gsl::czstring caScriptName, gsl::czstring caNameSpaceName)
{
    // KRodin: обращаться к _G только с большой буквы! Иначе он загрузится ещё раз и это неизвестно к чему приведёт!
    // Глобальное пространство инитится один раз после запуска луаджита, и никогда больше.
    if (std::is_eq(xr_strcmp(caNameSpaceName, "_g")))
        return false;

    const auto l_tpFileReader = absl::WrapUnique(FS.r_open(caScriptName));
    if (!l_tpFileReader)
    {
        // заменить на ассерт?
        Msg("!![CScriptStorage::do_file] Cannot open file [%s]", caScriptName);
        return false;
    }

    l_tpFileReader->skip_bom(caScriptName);

    string_path l_caLuaFileName;
    xr_strconcat(l_caLuaFileName, "@", caScriptName); // KRodin: приводит путь к виду @f:\games\s.t.a.l.k.e.r\gamedata\scripts\***.script

    absl::string_view strbuf{static_cast<gsl::czstring>(l_tpFileReader->pointer()), gsl::narrow_cast<size_t>(l_tpFileReader->elapsed())};
    xr_string script;

    if (std::is_neq(xr_strcmp(caNameSpaceName, GlobalNamespace)))
    {
        script = std::format(FILE_HEADER, caNameSpaceName, strbuf);
        strbuf = script;
    }

    lua().script(strbuf, l_caLuaFileName);

    return true;
}

// KRodin: видимо, функция проверяет, загружен ли скрипт.
bool CScriptStorage::namespace_loaded(gsl::czstring name, bool remove_from_stack)
{
    auto L = lua().lua_state();
    int start{
#ifdef DEBUG
        lua_gettop(L)
#endif
    };

    lua_pushstring(L, GlobalNamespace.data());
    lua_rawget(L, LUA_GLOBALSINDEX);
    string256 S2;
    xr_strcpy(S2, name);
    auto S = S2;
    for (;;)
    {
        if (!xr_strlen(S))
        {
            VERIFY(lua_gettop(L) >= 1);
            lua_pop(L, 1);
            VERIFY(start == lua_gettop(L));
            return false;
        }
        auto S1 = strchr(S, '.');
        if (S1)
            *S1 = 0;
        lua_pushstring(L, S);
        lua_rawget(L, -2);
        if (lua_isnil(L, -1))
        {
            // lua_settop(L,0);
            VERIFY(lua_gettop(L) >= 2);
            lua_pop(L, 2);
            VERIFY(start == lua_gettop(L));
            return false; // there is no namespace!
        }
        else if (!lua_istable(L, -1))
        {
            // lua_settop(L, 0);
            VERIFY(lua_gettop(L) >= 1);
            lua_pop(L, 1);
            VERIFY(start == lua_gettop(L));
            R_ASSERT3(false, "Error : the namespace is already being used by the non-table object! Name: ", S);
            return false;
        }
        lua_remove(L, -2);
        if (S1)
            S = ++S1;
        else
            break;
    }
    if (!remove_from_stack)
        VERIFY(lua_gettop(L) == start + 1);
    else
    {
        VERIFY(lua_gettop(L) >= 1);
        lua_pop(L, 1);
        VERIFY(lua_gettop(L) == start);
    }
    return true;
}
