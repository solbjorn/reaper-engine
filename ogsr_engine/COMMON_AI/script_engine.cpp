////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_engine.h"

#include "MainMenu.h"
#include "ai_space.h"
#include "object_factory.h"

#include "ui/UIWpnParams.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wcomma");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wunused-parameter");

#include <tracy/TracyLua.hpp>

XR_DIAG_POP();

namespace
{
// FILE_HEADER нужен для того, чтобы классы регистрировались внутри модуля в котором находятся, а не в _G
constexpr std::string_view FILE_HEADER{
    "\
local function script_name() \
return '{0}' \
end; \
local this; \
module('{0}', package.seeall, function(m) this = m end); \
{1}"};

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

int auto_load(lua_State* L)
{
    if (lua_gettop(L) < 2 || !lua_istable(L, 1) || !lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }

    std::ignore = ai().script_engine().process_file_if_exists(lua_tostring(L, 2), false);
    lua_rawget(L, 1);

    return 1;
}

bool initialized{};
} // namespace

CScriptEngine::CScriptEngine() = default;
CScriptEngine::~CScriptEngine() { close(); }

void CScriptEngine::setup_auto_load()
{
    auto L = lua().lua_state();

    lua_pushstring(L, GlobalNamespace.data());
    lua_gettable(L, LUA_GLOBALSINDEX);
    int value_index = lua_gettop(L); // alpet: во избежания оставления в стеке лишней метатаблицы

    luaL_newmetatable(L, "XRAY_AutoLoadMetaTable");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, auto_load);
    lua_settable(L, -3);

    // luaL_getmetatable(L, "XRAY_AutoLoadMetaTable");
    lua_setmetatable(L, value_index);

    xray_scripts.clear();
}

void CScriptEngine::register_script_classes()
{
    sol::function result = lua()["class_registrator"]["register"];
    result(const_cast<CObjectFactory*>(&object_factory()));
}

void CScriptEngine::reinit()
{
    close();

    m_virtual_machine.emplace();
    initialized = !!m_virtual_machine;

    std::ignore = Debug.set_lua_panic(lua_panic);
    std::ignore = Debug.set_lua_trace(ScriptCrashHandler);
}

void CScriptEngine::init()
{
    reinit();
    R_ASSERT(xr::script_engine_initialized(), "! ERROR : Cannot initialize LUA VM!");

    lua().open_libraries();
    tracy::LuaRegister(lua().lua_state());

    //-----------------------------------------------------//
    export_classes(lua()); // Тут регистрируются все движковые функции, импортированные в скрипты
    setup_auto_load(); // Построение метатаблицы

    const bool save = m_reload_modules;
    m_reload_modules = true;
    std::ignore = process_file_if_exists(GlobalNamespace.data(), false); // Компиляция _G.script
    m_reload_modules = save;

    register_script_classes(); // Походу, запуск class_registrator.script
    object_factory().register_script(); // Регистрация классов
}

void CScriptEngine::unload()
{
    destroy_lua_wpn_params();

    if (MainMenu())
        MainMenu()->DestroyInternal(true);

    xr_delete(g_object_factory);

    close(); // вызовем тут явно

    no_files.clear();
}

void CScriptEngine::close()
{
    if (!initialized)
        return;

    R_ASSERT(Debug.set_lua_trace(nullptr) == ScriptCrashHandler);
    R_ASSERT(Debug.set_lua_panic(nullptr) == lua_panic);

    initialized = false;
    m_virtual_machine.reset();
}

namespace xr
{
bool script_engine_initialized() { return initialized; }
} // namespace xr

namespace
{
gsl::czstring ExtractFileName(gsl::czstring fname)
{
    const auto pos = std::string_view{fname}.find_last_of('\\');

    return pos == std::string_view::npos ? fname : &fname[pos + 1];
}
} // namespace

void CScriptEngine::CollectScriptFiles(gsl::czstring path)
{
    if (xr_strlen(path) == 0)
        return;

    auto folders = FS.file_list_open(path, FS_ListFolders);
    if (folders == nullptr)
        goto files;

    std::ranges::for_each(*folders, [&](gsl::czstring folder) {
        if (std::strchr(folder, '.') != nullptr)
        {
            string_path fname;
            strconcat(sizeof(fname), fname, path, folder);

            CollectScriptFiles(fname);
        }
    });

    FS.file_list_close(folders);

files:
    auto files = FS.file_list_open(path, FS_ListFiles);
    if (files == nullptr)
        return;

    std::ranges::for_each(*files, [&](gsl::czstring file) {
        string_path fname;
        strconcat(sizeof(fname), fname, path, file);

        if (std::strstr(fname, ".script") != nullptr && FS.exist(fname) && FS.file_length(fname) > 0)
        {
            string_path buff;
            strcpy_s(buff, ExtractFileName(fname));
            _strlwr_s(buff);

            gsl::czstring nspace = std::strtok(buff, ".");
            const auto it = xray_scripts.find(nspace);

            R_ASSERT2(it == xray_scripts.end(), make_string("ERROR: script namespace \'%s\' conflict: %s vs %s", nspace, it->second.c_str(), fname));
            xray_scripts.emplace(nspace, fname);
        }
    });

    FS.file_list_close(files);
}

bool CScriptEngine::LookupScript(string_path& fname, gsl::czstring base)
{
    string_path lc_base;

    if (xray_scripts.empty())
    {
        std::ignore = FS.update_path(lc_base, "$game_scripts$", "");
        CollectScriptFiles(lc_base);
    }

    strcpy_s(lc_base, base);
    _strlwr_s(lc_base);

    auto it = xray_scripts.find(lc_base);
    if (it != xray_scripts.end())
    {
        strcpy_s(fname, it->second.c_str());
        return true;
    }

    return false;
}

bool CScriptEngine::namespace_loaded(gsl::czstring name, bool remove_from_stack)
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

bool CScriptEngine::do_file(gsl::czstring caScriptName, gsl::czstring caNameSpaceName)
{
    // KRodin: обращаться к _G только с большой буквы! Иначе он загрузится ещё раз и это неизвестно к чему приведёт!
    // Глобальное пространство инитится один раз после запуска луаджита, и никогда больше.
    if (std::is_eq(xr_strcmp(caNameSpaceName, "_g")))
        return false;

    const auto l_tpFileReader = absl::WrapUnique(FS.r_open(caScriptName));
    if (!l_tpFileReader)
    {
        // заменить на ассерт?
        Msg("!![CScriptEngine::do_file] Cannot open file [%s]", caScriptName);
        return false;
    }

    l_tpFileReader->skip_bom(caScriptName);

    string_path l_caLuaFileName;
    xr_strconcat(l_caLuaFileName, "@", caScriptName); // KRodin: приводит путь к виду @f:\games\s.t.a.l.k.e.r\gamedata\scripts\***.script

    std::string_view strbuf{static_cast<gsl::czstring>(l_tpFileReader->pointer()), gsl::narrow_cast<size_t>(l_tpFileReader->elapsed())};
    xr_string script;

    if (std::is_neq(xr_strcmp(caNameSpaceName, GlobalNamespace)))
    {
        script = std::format(FILE_HEADER, caNameSpaceName, strbuf);
        strbuf = script;
    }

    lua().script(strbuf, l_caLuaFileName);

    return true;
}

bool CScriptEngine::process_file_if_exists(gsl::czstring file_name, bool warn_if_not_exist)
{
    if (!warn_if_not_exist && no_file_exists(file_name)) // Это для оптимизации, чтоб постоянно не проверять, отсутствует ли этот файл.
        return false;
    if (m_reload_modules || (*file_name && !namespace_loaded(file_name)))
    {
        string_path S;
        if (!LookupScript(S, file_name))
        {
            if (warn_if_not_exist)
                MsgDbg("[CScriptEngine::process_file_if_exists] Variable %s not found; No script by this name exists, either.", file_name);
            else
            {
                LogDbg("-------------------------");
                MsgDbg("[CScriptEngine::process_file_if_exists] WARNING: Access to nonexistent variable or loading nonexistent script '%s'", file_name);
                FuncDbg(print_stack());
                LogDbg("-------------------------");
                add_no_file(file_name);
            }
            return false;
        }
#ifdef DEBUG
        MsgDbg("[CScriptEngine::process_file_if_exists] loading script: [%s]", file_name);
#endif
        m_reload_modules = false;
        return do_file(S, file_name);
    }
    return true;
}

bool CScriptEngine::process_file(gsl::czstring file_name, bool reload_modules)
{
    m_reload_modules = reload_modules;
    bool result = process_file_if_exists(file_name, true);
    m_reload_modules = false;

    return result;
}

void CScriptEngine::parse_script_namespace(gsl::czstring name, gsl::zstring ns, u32 nsSize, gsl::zstring func, u32 funcSize)
{
    auto p = std::strrchr(name, '.');
    if (p == nullptr)
    {
        xr_strcpy(ns, nsSize, GlobalNamespace.data());
        p = name - 1;
    }
    else
    {
        VERIFY(u32(p - name + 1) <= nsSize);

        std::strncpy(ns, name, p - name);
        ns[p - name] = '\0';
    }

    xr_strcpy(func, funcSize, p + 1);
}

bool CScriptEngine::function(gsl::czstring function_to_call, sol::function& func)
{
    if (xr_strlen(function_to_call) == 0)
        return false;

    string256 name_space, function;
    parse_script_namespace(function_to_call, name_space, sizeof(name_space), function, sizeof(function));

    if (std::is_neq(xr_strcmp(name_space, GlobalNamespace)))
    {
        auto file_name = std::strchr(name_space, '.');
        if (file_name == nullptr)
        {
            std::ignore = process_file(name_space);
        }
        else
        {
            *file_name = '\0';
            std::ignore = process_file(name_space);
            *file_name = '.';
        }
    }

    gsl::czstring ns = name_space;
    gsl::czstring fn = function;

    auto x = lua().get<sol::optional<sol::function>>(std::tie(ns, fn));
    if (!x)
        return false;

    func = x.value();
    return true;
}

void CScriptEngine::collect_all_garbage()
{
    for (gsl::index i{0}; i < 4; ++i)
        lua().collect_gc();
}

//*********************************************************************************************

void CScriptEngine::print_stack()
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

void CScriptEngine::dump_state()
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

        if (std::is_eq(xr_strcmp(l_tDebugInfo.what, "C")))
        {
            Msg("%2d : [C  ] %s", i, l_tDebugInfo.name ? l_tDebugInfo.name : "");
        }
        else
        {
            string_path temp;

            if (l_tDebugInfo.name)
                xr_sprintf(temp, "%s(%d)", l_tDebugInfo.name, l_tDebugInfo.linedefined);
            else
                xr_sprintf(temp, "function <%s:%d>", l_tDebugInfo.short_src, l_tDebugInfo.linedefined);

            Msg("%2d : [%3s] %s(%d) : %s", i, l_tDebugInfo.what, l_tDebugInfo.short_src, l_tDebugInfo.currentline, temp);
        }

        Msg("\tLocals:");

        gsl::czstring name;
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

void CScriptEngine::LogTable(lua_State* l, LPCSTR S, int level)
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

void CScriptEngine::LogVariable(lua_State* l, gsl::czstring name, int level)
{
    gsl::czstring type;
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

#ifdef DEBUG
// Используется в очень многих местах //Очень много пишет в лог.
void CScriptEngine::script_log(ScriptStorage::ELuaMessageType tLuaMessageType, gsl::czstring caFormat, ...)
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
