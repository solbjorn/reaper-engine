// File:        UIComboBox.h
// Description: guess :)
// Created:     10.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#pragma once

#include "UIEditBox.h"
//.#include "UI3tButton.h"
// #include "UIListWnd.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "UIInteractiveBackground.h"
#include "UIOptionsItem.h"

class CUIListBoxItem;

class CUIComboBox : public CUIWindow, public CUIOptionsItem
{
    RTTI_DECLARE_TYPEINFO(CUIComboBox, CUIWindow, CUIOptionsItem);

public:
    friend class CUIXmlInit;
    typedef enum
    {
        LIST_EXPANDED,
        LIST_FONDED
    } E_COMBO_STATE;

    CUIComboBox();
    ~CUIComboBox() override;

    // CUIOptionsItem
    void SetCurrentValue() override;
    void SaveValue() override;
    [[nodiscard]] bool IsChanged() override;
    void SeveBackUpValue() override;
    void Undo() override;

    [[nodiscard]] gsl::czstring GetText();

    // methods
    CUIListBox* GetListWnd();
    void SetListLength(int length);
    void SetVertScroll(bool bVScroll = true) { m_list.SetFixedScrollBar(bVScroll); }
    CUIListBoxItem* AddItem_(LPCSTR str, int _data);
    CUIListBoxItem* AddItem_(LPCSTR str);
    virtual void Init(float x, float y, float width);
    void SetItem(int i);

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr);
    void OnFocusLost() override;
    void OnFocusReceive() override;

    int CurrentID() { return m_itoken_id; }

    void SetFont(CGameFont* pFont) override;

protected:
    virtual void SetState(UIState state);
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;
    virtual void OnBtnClicked();
    void ShowList(bool bShow);
    void OnListItemSelect();
    void Update() override;

protected:
    bool m_bInited;
    int m_iListHeight;
    int m_itoken_id{};
    E_COMBO_STATE m_eState;
    int m_backup_itoken_id{};

    CUI_IB_FrameLineWnd m_frameLine;
    CUILabel m_text;
    CUIFrameWindow m_frameWnd;

    u32 m_textColor[2];

public:
    CUIListBox m_list;
    void SetTextColor(u32 color) { m_textColor[0] = color; }
    void SetTextColorD(u32 color) { m_textColor[1] = color; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CUIComboBox);

add_to_type_list(CUIComboBox);
#undef script_type_list
#define script_type_list save_type_list(CUIComboBox)
