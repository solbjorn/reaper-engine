#pragma once

#include "UILabel.h"
#include "UIOptionsItem.h"

class CUIColorAnimatorWrapper;
struct _action;
struct _keyboard;

class CUIEditKeyBind : public CUILabel, public CUIOptionsItem
{
    RTTI_DECLARE_TYPEINFO(CUIEditKeyBind, CUILabel, CUIOptionsItem);

public:
    _action* m_action{};
    _keyboard* m_keyboard{};
    bool m_bPrimary;

    explicit CUIEditKeyBind(bool bPrim);
    ~CUIEditKeyBind() override;

    // options item
    void Register(gsl::czstring entry, gsl::czstring group) override;
    void SetCurrentValue() override;
    void SaveValue() override;
    void OnMessage(gsl::czstring message) override;
    [[nodiscard]] bool IsChanged() override;

    // CUIWindow methods
    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void Update() override;
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button mouse_btn) override;
    void OnFocusLost() override;
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;
    // IUITextControl
    void SetText(gsl::czstring text) override;

protected:
    void BindAction2Key();
    void InitTexture(gsl::czstring texture, bool horizontal = true) override;

    bool m_bEditMode{};
    bool m_bChanged{};

    CUIColorAnimatorWrapper* m_pAnimation;
};
