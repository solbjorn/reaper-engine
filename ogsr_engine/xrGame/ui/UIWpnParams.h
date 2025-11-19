#pragma once

#include "../script_export_space.h"
#include "UIProgressBar.h"
#include "UIWindow.h"

class CPhysicsShellHolder;
class CUIXml;

class CUIWpnParams : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIWpnParams, CUIWindow);

public:
    CUIWpnParams();
    ~CUIWpnParams() override;

    void InitFromXml(CUIXml& xml_doc);
    void SetInfo(CPhysicsShellHolder& obj /*const shared_str& wpn_section*/);
    bool Check(CPhysicsShellHolder& obj /*const shared_str& wpn_section*/);

protected:
    CUIProgressBar m_progressAccuracy;
    CUIProgressBar m_progressHandling;
    CUIProgressBar m_progressDamage;
    CUIProgressBar m_progressRPM;

    CUIStatic m_textAccuracy;
    CUIStatic m_textHandling;
    CUIStatic m_textDamage;
    CUIStatic m_textRPM;
};

void destroy_lua_wpn_params();
