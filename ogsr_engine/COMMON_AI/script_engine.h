////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

namespace ScriptStorage
{
enum ELuaMessageType : u32
{
    eLuaMessageTypeInfo = u32(0),
    eLuaMessageTypeError,
    eLuaMessageTypeMessage,
    eLuaMessageTypeHookCall,
    eLuaMessageTypeHookReturn,
    eLuaMessageTypeHookLine,
    eLuaMessageTypeHookCount,
};
}

class CScriptEngine : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CScriptEngine);

private:
    alignas(TMC_CACHE_LINE_SIZE) std::optional<sol::state> m_virtual_machine;

    string_unordered_map<shared_str, shared_str> xray_scripts;
    string_unordered_map<shared_str, bool> no_files;

    bool m_reload_modules{};
    xr_set<void*> m_dumpedObjList;

    [[nodiscard]] bool do_file(gsl::czstring caScriptName, gsl::czstring caNameSpaceName);
    [[nodiscard]] bool namespace_loaded(gsl::czstring caName, bool remove_from_stack = true);

    void CollectScriptFiles(gsl::czstring path);
    [[nodiscard]] bool LookupScript(string_path& fname, gsl::czstring base);

    [[nodiscard]] bool no_file_exists(gsl::czstring file_name) const { return no_files.contains(file_name); }
    void add_no_file(gsl::czstring file_name) { no_files.emplace(file_name, true); }

    void reinit();
    void close();

public:
    CScriptEngine();
    ~CScriptEngine() override;

    static constexpr std::string_view GlobalNamespace{"_G"};

    void setup_auto_load();
    void init();
    virtual void unload();

    [[nodiscard]] sol::state_view& lua() { return *m_virtual_machine; }
    [[nodiscard]] const sol::state_view& lua() const { return *m_virtual_machine; }

    [[nodiscard]] bool process_file_if_exists(gsl::czstring file_name, bool warn_if_not_exist);
    [[nodiscard]] bool process_file(gsl::czstring file_name) { return process_file_if_exists(file_name, true); }
    [[nodiscard]] bool process_file(gsl::czstring file_name, bool reload_modules);

    void parse_script_namespace(gsl::czstring name, gsl::zstring ns, u32 nsSize, gsl::zstring func, u32 funcSize);
    [[nodiscard]] bool function(gsl::czstring function_to_call, sol::function& func);

    void register_script_classes();
    void collect_all_garbage();

    void print_stack();
    void dump_state();
    void LogTable(lua_State* l, LPCSTR S, int level);
    void LogVariable(lua_State* l, gsl::czstring name, int level);

#ifdef DEBUG
    void XR_PRINTF(3, 4) script_log(ScriptStorage::ELuaMessageType message, gsl::czstring caFormat, ...);
#else
    static void XR_PRINTF(2, 3) script_log(ScriptStorage::ELuaMessageType, gsl::czstring, ...) {}
#endif

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CScriptEngine);
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)

namespace xr
{
[[nodiscard]] bool script_engine_initialized();
}

// script_engine_export.cpp
void export_classes(sol::state_view& lua);

// script_engine_script.cpp
bool GetShift();
