////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////
#pragma once

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

class CScriptStorage : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CScriptStorage);

private:
    alignas(std::hardware_destructive_interference_size) std::optional<sol::state> m_virtual_machine;
    xr_set<void*> m_dumpedObjList;

protected:
    [[nodiscard]] bool initialized() const { return !!m_virtual_machine; }

    [[nodiscard]] bool do_file(gsl::czstring caScriptName, gsl::czstring caNameSpaceName);
    [[nodiscard]] bool namespace_loaded(gsl::czstring caName, bool remove_from_stack = true);
    void reinit();
    void close();

public:
    CScriptStorage() = default;
    ~CScriptStorage() override;

    static constexpr absl::string_view GlobalNamespace{"_G"};

    [[nodiscard]] sol::state_view& lua() { return *m_virtual_machine; }
    [[nodiscard]] const sol::state_view& lua() const { return *m_virtual_machine; }

#ifdef DEBUG
    void XR_PRINTF(3, 4) script_log(ScriptStorage::ELuaMessageType message, const char* caFormat, ...);
#else
    static void XR_PRINTF(2, 3) script_log(ScriptStorage::ELuaMessageType, const char*, ...) {}
#endif

    void print_stack();
    void dump_state();
    void LogTable(lua_State* l, LPCSTR S, int level);
    void LogVariable(lua_State* l, const char* name, int level);
};
