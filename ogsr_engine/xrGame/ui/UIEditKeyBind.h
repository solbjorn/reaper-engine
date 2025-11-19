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
    virtual void Register(const char* entry, const char* group);
    virtual void SetCurrentValue();
    virtual void SaveValue();
    virtual void OnMessage(const char* message);
    virtual bool IsChanged();

    // CUIWindow methods
    virtual void Init(float x, float y, float width, float height);
    virtual void Update();
    virtual bool OnMouseDown(int mouse_btn);
    virtual void OnFocusLost();
    virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
    // IUITextControl
    virtual void SetText(const char* text);

protected:
    void BindAction2Key();
    virtual void InitTexture(LPCSTR texture, bool horizontal = true);

    bool m_bEditMode{};
    bool m_bChanged{};

    CUIColorAnimatorWrapper* m_pAnimation;
};
