////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_engine.h"

#include "ai_space.h"
#include "MainMenu.h"
#include "object_factory.h"

#include "ui/UIWpnParams.h"

CScriptEngine::CScriptEngine() = default;
CScriptEngine::~CScriptEngine() = default;

void CScriptEngine::unload()
{
    destroy_lua_wpn_params();
    if (MainMenu())
    {
        MainMenu()->DestroyInternal(true);
    }

    xr_delete(g_object_factory);

    close(); // вызовем тут явно

    no_files.clear();
}

namespace
{
int auto_load(lua_State* L)
{
    if ((lua_gettop(L) < 2) || !lua_istable(L, 1) || !lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }
    ai().script_engine().process_file_if_exists(lua_tostring(L, 2), false);
    lua_rawget(L, 1);
    return 1;
}

string_unordered_map<shared_str, shared_str> xray_scripts;
} // namespace

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

void CScriptEngine::init()
{
    reinit();
    R_ASSERT(xr::script_engine_initialized(), "! ERROR : Cannot initialize LUA VM!");
    lua().open_libraries();

    //-----------------------------------------------------//
    export_classes(lua()); // Тут регистрируются все движковые функции, импортированные в скрипты
    setup_auto_load(); // Построение метатаблицы

    const bool save = m_reload_modules;
    m_reload_modules = true;
    process_file_if_exists(GlobalNamespace.data(), false); // Компиляция _G.script
    m_reload_modules = save;

    register_script_classes(); // Походу, запуск class_registrator.script
    object_factory().register_script(); // Регистрация классов
}

void CScriptEngine::parse_script_namespace(const char* name, char* ns, u32 nsSize, char* func, u32 funcSize)
{
    auto p = strrchr(name, '.');
    if (!p)
    {
        xr_strcpy(ns, nsSize, GlobalNamespace.data());
        p = name - 1;
    }
    else
    {
        VERIFY(u32(p - name + 1) <= nsSize);
        strncpy(ns, name, p - name);
        ns[p - name] = 0;
    }
    xr_strcpy(func, funcSize, p + 1);
}

namespace
{
const char* ExtractFileName(const char* fname)
{
    const auto pos = absl::string_view{fname}.find_last_of('\\');

    return pos == absl::string_view::npos ? fname : &fname[pos + 1];
}

void CollectScriptFiles(decltype(xray_scripts)& map, const char* path)
{
    if (xr_strlen(path) == 0)
        return;

    string_path fname;
    auto folders = FS.file_list_open(path, FS_ListFolders);
    if (folders)
    {
        std::for_each(folders->begin(), folders->end(), [&](const char* folder) {
            if (strchr(folder, '.'))
            {
                strconcat(sizeof(fname), fname, path, folder);
                CollectScriptFiles(map, fname);
            }
        });
        FS.file_list_close(folders);
    }

    string_path buff;
    auto files = FS.file_list_open(path, FS_ListFiles);
    if (!files)
        return;
    std::for_each(files->begin(), files->end(), [&](const char* file) {
        strconcat(sizeof(fname), fname, path, file);
        if (strstr(fname, ".script") && FS.exist(fname) && FS.file_length(fname) > 0)
        {
            const char* fstart = ExtractFileName(fname);
            strcpy_s(buff, fstart);
            _strlwr_s(buff);

            const char* nspace = strtok(buff, ".");
            auto it = map.find(nspace);
            R_ASSERT2(it == map.end(), make_string("ERROR: script namespace \'%s\' conflict: %s vs %s", nspace, it->second.c_str(), fname));
            map.emplace(nspace, fname);
        }
    });
    FS.file_list_close(files);
}

bool LookupScript(string_path& fname, const char* base)
{
    string_path lc_base;

    if (xray_scripts.empty())
    {
        std::ignore = FS.update_path(lc_base, "$game_scripts$", "");
        CollectScriptFiles(xray_scripts, lc_base);
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
} // namespace

bool CScriptEngine::process_file_if_exists(const char* file_name, bool warn_if_not_exist) // KRodin: Функция проверяет существует ли скрипт на диске. Если существует - отправляет
                                                                                          // его в do_file. Вызывается из process_file, auto_load и не только.
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

bool CScriptEngine::process_file(const char* file_name) { return process_file_if_exists(file_name, true); }

bool CScriptEngine::process_file(const char* file_name, bool reload_modules)
{
    m_reload_modules = reload_modules;
    bool result = process_file_if_exists(file_name, true);
    m_reload_modules = false;
    return result;
}

void CScriptEngine::register_script_classes()
{
    sol::function result = lua()["class_registrator"]["register"];
    result(const_cast<CObjectFactory*>(&object_factory()));
}

bool CScriptEngine::function(gsl::czstring function_to_call, sol::function& func)
{
    if (xr_strlen(function_to_call) == 0)
        return false;

    string256 name_space, function;
    parse_script_namespace(function_to_call, name_space, sizeof(name_space), function, sizeof(function));

    if (std::is_neq(xr_strcmp(name_space, GlobalNamespace)))
    {
        auto file_name = strchr(name_space, '.');
        if (!file_name)
            process_file(name_space);
        else
        {
            *file_name = 0;
            process_file(name_space);
            *file_name = '.';
        }
    }

    const char* ns = name_space;
    const char* fn = function;

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
