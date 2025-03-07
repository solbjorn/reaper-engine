#include "stdafx.h"
#include "UIButton.h"
#include "UI3tButton.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UISpinNum.h"
#include "UISpinText.h"
#include "UITrackBar.h"

using namespace luabind;

void CUIButton::script_register(lua_State* L)
{
    module(L)[(class_<CUIButton, CUIStatic>("CUIButton")
                   .def(constructor<>())
                   .def("Init", (void(CUIButton::*)(float, float, float, float)) & CUIButton::Init)
                   .def("Init", (void(CUIButton::*)(LPCSTR, float, float, float, float)) & CUIButton::Init)
                   .def("SetHighlightColor", &CUIButton::SetHighlightColor)
                   .def("EnableTextHighlighting", &CUIButton::EnableTextHighlighting)
                   .def("SetAccelerator", &CUIButton::SetAccelerator),

               class_<CUI3tButton, CUIButton>("CUI3tButton").def(constructor<>()),

               class_<CUICheckButton, CUI3tButton>("CUICheckButton")
                   .def(constructor<>())
                   .def("GetCheck", &CUICheckButton::GetCheck)
                   .def("SetCheck", &CUICheckButton::SetCheck)
                   .def("SetDependControl", &CUICheckButton::SetDependControl),

               class_<CUICustomSpin, CUIWindow>("CUICustomSpin").def("Init", &CUICustomSpin::Init).def("GetText", &CUICustomSpin::GetText),

               class_<CUISpinNum, CUICustomSpin>("CUISpinNum").def(constructor<>()),

               class_<CUISpinFlt, CUICustomSpin>("CUISpinFlt").def(constructor<>()),

               class_<CUISpinText, CUICustomSpin>("CUISpinText").def(constructor<>()),

               class_<CUITrackBar, CUIWindow>("CUITrackBar")
                   .def(constructor<>())
                   .def("GetCheck", &CUITrackBar::GetCheck)
                   .def("SetCheck", &CUITrackBar::SetCheck)
                   .def("GetTrackValue", &CUITrackBar::GetTrackValue))];
}
