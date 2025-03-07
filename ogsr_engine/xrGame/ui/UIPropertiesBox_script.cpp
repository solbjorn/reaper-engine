#include "stdafx.h"
#include "UIPropertiesBox.h"

using namespace luabind;

void CUIPropertiesBox::script_register(lua_State* L)
{
    module(L)[class_<CUIPropertiesBox, CUIFrameWindow>("CUIPropertiesBox")
                  .def(constructor<>())
                  .def("RemoveItem", &CUIPropertiesBox::RemoveItemByTAG)
                  .def("RemoveAll", &CUIPropertiesBox::RemoveAll)
                  .def("Show", (void(CUIPropertiesBox::*)(int, int)) & CUIPropertiesBox::Show)
                  .def("Hide", &CUIPropertiesBox::Hide)
                  .def("AutoUpdateSize", &CUIPropertiesBox::AutoUpdateSize)
                  .def("AddItem", &CUIPropertiesBox::AddItem_script)];
}
