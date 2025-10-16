#include "stdafx.h"

#include "UIMainIngameWnd.h"
#include "UICarPanel.h"
#include "UIXmlInit.h"

constexpr LPCSTR CAR_PANEL_XML = "car_panel.xml";

void CUICarPanel::Init(float x, float y, float width, float height)
{
    CUIXml uiXml;
    bool result = uiXml.Init(CONFIG_PATH, UI_PATH, CAR_PANEL_XML);
    R_ASSERT3(result, "xml file not found", CAR_PANEL_XML);

    CUIXmlInit xml_init;
    ////////////////////////////////////////////////////////////////////
    AttachChild(&UIStaticCarHealth);
    xml_init.InitStatic(uiXml, "car_health_static", 0, &UIStaticCarHealth);

    UIStaticCarHealth.AttachChild(&UICarHealthBar);
    xml_init.InitProgressBar(uiXml, "car_health_progress_bar", 0, &UICarHealthBar);

    Show(false);
    Enable(false);

    inherited::Init(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUICarPanel::SetCarHealth(float value)
{
    float pos = value * 100.f;
    clamp(pos, 0.0f, 100.0f);
    UICarHealthBar.SetProgressPos(pos);
}
