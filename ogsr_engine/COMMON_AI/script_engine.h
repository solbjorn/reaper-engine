////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_storage.h"
#include "script_export_space.h"

class CScriptEngine : public CScriptStorage
{
    RTTI_DECLARE_TYPEINFO(CScriptEngine, CScriptStorage);

private:
    bool m_reload_modules{};

    string_unordered_map<shared_str, bool> no_files;

    inline bool no_file_exists(const char* file_name) const { return no_files.contains(file_name); }
    inline void add_no_file(const char* file_name) { no_files.emplace(file_name, true); }

public:
    CScriptEngine();
    ~CScriptEngine() override;

    void init();
    virtual void unload();
    void setup_auto_load();

    bool process_file_if_exists(const char* file_name, bool warn_if_not_exist);
    bool process_file(const char* file_name);
    bool process_file(const char* file_name, bool reload_modules);
    [[nodiscard]] bool function(gsl::czstring function_to_call, sol::function& func);
    void parse_script_namespace(const char* name, char* ns, u32 nsSize, char* func, u32 funcSize);

    void register_script_classes();
    void collect_all_garbage();

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CScriptEngine);
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)

// script_engine_export.cpp
void export_classes(sol::state_view& lua);

// script_engine_script.cpp
bool GetShift();
