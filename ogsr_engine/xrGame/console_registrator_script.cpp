#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "../xr_3da/XR_IOConsole.h"
#include "../xr_3da/xr_ioc_cmd.h"

#include "console_registrator.h"

static CConsole* console() { return Console; }

static int get_console_integer(CConsole* c, LPCSTR cmd)
{
    int val = 0, min = 0, max = 0;
    val = c->GetInteger(cmd, min, max);
    return val;
}

static float get_console_float(CConsole* c, LPCSTR cmd)
{
    float val = 0, min = 0, max = 0;
    val = c->GetFloat(cmd, min, max);
    return val;
}

static bool get_console_bool(CConsole* c, LPCSTR cmd)
{
    BOOL val;
    val = c->GetBool(cmd);
    return !!val;
}

static IConsole_Command* find_cmd(CConsole* c, LPCSTR cmd)
{
    CConsole::vecCMD_IT I = c->Commands.find(cmd);
    IConsole_Command* icmd = NULL;

    if (I != c->Commands.end())
        icmd = I->second;

    return icmd;
}

static void disable_cmd(CConsole* c, LPCSTR cmd)
{
    IConsole_Command* icmd = find_cmd(c, cmd);
    if (icmd)
        icmd->SetEnabled(false);
}

static void enable_cmd(CConsole* c, LPCSTR cmd)
{
    IConsole_Command* icmd = find_cmd(c, cmd);
    if (icmd)
        icmd->SetEnabled(true);
}

void console_registrator::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.set_function("get_console", &console);

    lua.new_usertype<CConsole>("CConsole", sol::no_constructor, "disable_command", &disable_cmd, "enable_command", &enable_cmd, "execute",
                               sol::overload(sol::resolve<void(LPCSTR)>(&CConsole::Execute), sol::resolve<void(LPCSTR, LPCSTR)>(&CConsole::Execute)), "execute_script",
                               &CConsole::ExecuteScript, "show", &CConsole::Show, "hide", &CConsole::Hide, "get_string", &CConsole::GetString, "get_integer", &get_console_integer,
                               "get_bool", &get_console_bool, "get_float", &get_console_float, "get_token", &CConsole::GetToken, "get_vector", &CConsole::GetFVector, "get_vector4",
                               &CConsole::GetFVector4, "visible", sol::readonly(&CConsole::bVisible));
}
