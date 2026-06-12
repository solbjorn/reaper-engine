#pragma once

#include "ui3tbutton.h"
#include "UIOptionsItem.h"

class CUICheckButton : public CUI3tButton, public CUIOptionsItem
{
    RTTI_DECLARE_TYPEINFO(CUICheckButton, CUI3tButton, CUIOptionsItem);

public:
    CUICheckButton();
    ~CUICheckButton() override;

    void Update() override;

    // CUIOptionsItem
    void SetCurrentValue() override;
    void SaveValue() override;
    [[nodiscard]] bool IsChanged() override;
    void SeveBackUpValue() override;
    void Undo() override;

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void InitTexture(gsl::czstring tex_name) override;
    void SetTextX(f32) override {}

    // состояние кнопки
    [[nodiscard]] bool GetCheck() { return m_eButtonState == BUTTON_PUSHED; }

    void SetCheck(bool ch)
    {
        m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL;
        GetMessageTarget()->SendMessage(this, ch ? CHECK_BUTTON_SET : CHECK_BUTTON_RESET);
    }

    void SetDependControl(CUIWindow* pWnd);

private:
    bool b_backup_val{};
    CUIWindow* m_pDependControl{};
};
XR_SOL_BASE_CLASSES(CUICheckButton);
