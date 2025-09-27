#include "StdAfx.h"

#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "../Level.h"
#include "../ai_space.h"
#include "../script_engine.h"
#include "../PhysicsShellHolder.h"
#include "clsid_game.h"
#include "script_game_object.h"

namespace
{
struct SLuaWpnParams
{
    sol::function m_functionRPM;
    sol::function m_functionAccuracy;
    sol::function m_functionDamage;
    sol::function m_functionHandling;

    SLuaWpnParams();
    ~SLuaWpnParams();
};

SLuaWpnParams::SLuaWpnParams()
{
    auto& lua = ai().script_engine();
    bool function_exists;

    function_exists = lua.function("ui_wpn_params.GetRPM", m_functionRPM);
    VERIFY(function_exists);
    function_exists = lua.function("ui_wpn_params.GetDamage", m_functionDamage);
    VERIFY(function_exists);
    function_exists = lua.function("ui_wpn_params.GetHandling", m_functionHandling);
    VERIFY(function_exists);
    function_exists = lua.function("ui_wpn_params.GetAccuracy", m_functionAccuracy);
    VERIFY(function_exists);
}

SLuaWpnParams::~SLuaWpnParams() {}

SLuaWpnParams* g_lua_wpn_params;
} // namespace

void destroy_lua_wpn_params()
{
    if (g_lua_wpn_params)
        xr_delete(g_lua_wpn_params);
}

CUIWpnParams::CUIWpnParams()
{
    AttachChild(&m_textAccuracy);
    AttachChild(&m_textDamage);
    AttachChild(&m_textHandling);
    AttachChild(&m_textRPM);

    AttachChild(&m_progressAccuracy);
    AttachChild(&m_progressDamage);
    AttachChild(&m_progressHandling);
    AttachChild(&m_progressRPM);
}

CUIWpnParams::~CUIWpnParams() {}

void CUIWpnParams::InitFromXml(CUIXml& xml_doc)
{
    if (!xml_doc.NavigateToNode("wpn_params", 0))
        return;

    CUIXmlInit::InitWindow(xml_doc, "wpn_params", 0, this);

    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_accuracy", 0, &m_textAccuracy);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage", 0, &m_textDamage);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_handling", 0, &m_textHandling);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_rpm", 0, &m_textRPM);

    CUIXmlInit::InitProgressBar(xml_doc, "wpn_params:progress_accuracy", 0, &m_progressAccuracy);
    CUIXmlInit::InitProgressBar(xml_doc, "wpn_params:progress_damage", 0, &m_progressDamage);
    CUIXmlInit::InitProgressBar(xml_doc, "wpn_params:progress_handling", 0, &m_progressHandling);
    CUIXmlInit::InitProgressBar(xml_doc, "wpn_params:progress_rpm", 0, &m_progressRPM);

    m_progressAccuracy.SetRange(0, 100);
    m_progressDamage.SetRange(0, 100);
    m_progressHandling.SetRange(0, 100);
    m_progressRPM.SetRange(0, 100);
}

void CUIWpnParams::SetInfo(CPhysicsShellHolder& obj /*const shared_str& wpn_section*/)
{
    if (!g_lua_wpn_params)
        g_lua_wpn_params = xr_new<SLuaWpnParams>();

    const shared_str& wpn_section = obj.cNameSect();

    m_progressRPM.SetProgressPos(g_lua_wpn_params->m_functionRPM(*wpn_section, obj.lua_game_object()));
    m_progressAccuracy.SetProgressPos(g_lua_wpn_params->m_functionAccuracy(*wpn_section, obj.lua_game_object()));
    m_progressDamage.SetProgressPos(g_lua_wpn_params->m_functionDamage(*wpn_section, obj.lua_game_object()));
    m_progressHandling.SetProgressPos(g_lua_wpn_params->m_functionHandling(*wpn_section, obj.lua_game_object()));
}

bool CUIWpnParams::Check(CPhysicsShellHolder& obj /*const shared_str& wpn_section*/)
{
    if (!READ_IF_EXISTS(pSettings, r_bool, obj.cNameSect(), "show_wpn_properties", true)) // allow to suppress default wpn params
    {
        return false;
    }

    if (pSettings->line_exist(obj.cNameSect(), "fire_dispersion_base"))
    {
        /*
        if (0==xr_strcmp(wpn_section, "wpn_addon_silencer"))
            return false;
        if (0==xr_strcmp(wpn_section, "wpn_binoc"))
            return false;
        if (0==xr_strcmp(wpn_section, "mp_wpn_binoc"))
            return false;*/

        if (obj.CLS_ID == CLSID_OBJECT_W_BINOCULAR || obj.CLS_ID == CLSID_OBJECT_W_KNIFE || obj.CLS_ID == CLSID_OBJECT_W_SILENCER || obj.CLS_ID == CLSID_OBJECT_W_SCOPE ||
            obj.CLS_ID == CLSID_OBJECT_W_GLAUNCHER)
            return false;

        return true;
    }
    else
        return false;
}
