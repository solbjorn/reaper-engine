// CUIEditBox.h: ввод строки с клавиатуры
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UILabel.h"
#include "../script_export_space.h"
#include "UIOptionsItem.h"
#include "UIColorAnimatorWrapper.h"
#include "UICustomEdit.h"

//////////////////////////////////////////////////////////////////////////

class game_cl_GameState;

//////////////////////////////////////////////////////////////////////////

class CUIEditBox : public CUIMultiTextureOwner, public CUIOptionsItem, public CUICustomEdit
{
    RTTI_DECLARE_TYPEINFO(CUIEditBox, CUIMultiTextureOwner, CUIOptionsItem, CUICustomEdit);

public:
    CUIEditBox();
    ~CUIEditBox() override;

    void Init(f32 x, f32 y, f32 width, f32 heigt) override;

    // CUIOptionsItem
    void SetCurrentValue() override;
    void SaveValue() override;
    [[nodiscard]] bool IsChanged() override;

    // CUIMultiTextureOwner
    void InitTexture(gsl::czstring texture) override;

protected:
    CUIFrameLineWnd m_frameLine;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CUIEditBox);

add_to_type_list(CUIEditBox);
#undef script_type_list
#define script_type_list save_type_list(CUIEditBox)
