// File:        UI3tButton.cpp
// Description: Button with 3 texutres (for <enabled>, <disabled> and <touched> states)
// Created:     07.12.2004
// Author:      Serhiy 0. Vynnychenk0
// Mail:        narrator@gsc-game.kiev.ua
//
// copyright 2004 GSC Game World
//

#pragma once

#include "UIButton.h"
#include "UI_IB_Static.h"

class CUI3tButton : public CUIButton
{
    RTTI_DECLARE_TYPEINFO(CUI3tButton, CUIButton);

public:
    friend class CUIXmlInit;
    using CUIButton::SetTextColor;

    CUI3tButton();
    ~CUI3tButton() override;

    // appearance
    using CUIButton::Init;

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void InitTexture(gsl::czstring tex_name) override;
    virtual void InitTexture(gsl::czstring tex_enabled, gsl::czstring tex_disabled, gsl::czstring tex_touched, gsl::czstring tex_highlighted);

    void SetTextColor(u32 color) override;
    void SetTextColorH(u32 color);
    void SetTextColorD(u32 color);
    void SetTextColorT(u32 color);
    void SetTextureOffset(f32 x, f32 y) override;
    void SetWidth(f32 width) override;
    void SetHeight(f32 height) override;
    void InitSoundH(LPCSTR sound_file);
    void InitSoundT(LPCSTR sound_file);

    void OnClick() override;
    void OnFocusReceive() override;
    void OnFocusLost() override;

    // check button
    [[nodiscard]] bool GetCheck() { return m_eButtonState == BUTTON_PUSHED; }
    void SetCheck(bool ch) { m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL; }

    // behavior
    void DrawTexture() override;
    void Update() override;

    // virtual void Enable(bool bEnable);
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button mouse_btn) override;
    void SetCheckMode(bool mode) { m_bCheckMode = mode; }

    void SetStretchTexture(bool stretch_texture) override;
    void EnableHeading(bool b) override;

    CUIStatic m_hint;
    CUI_IB_Static m_background;

protected:
    bool m_bCheckMode;

private:
    void PlaySoundH();
    void PlaySoundT();

    ref_sound m_sound_h;
    ref_sound m_sound_t;
};
XR_SOL_BASE_CLASSES(CUI3tButton);
