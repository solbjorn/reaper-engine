#pragma once

#include "uiframewindow.h"
#include "uilistbox.h"

#include "../script_export_space.h"

class CUIPropertiesBox : public CUIFrameWindow
{
    RTTI_DECLARE_TYPEINFO(CUIPropertiesBox, CUIFrameWindow);

private:
    typedef CUIFrameWindow inherited;

public:
    CUIPropertiesBox();
    ~CUIPropertiesBox() override;

    virtual void Init(float x, float y, float width, float height);

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;
    [[nodiscard]] bool OnKeyboard(xr::key_id, EUIMessages) override;

    bool AddItem(const char* str, void* pData = nullptr, u32 tag_value = 0);
    bool AddItem_script(const char* str) { return AddItem(str); }
    u32 GetItemsCount() { return m_UIListWnd.GetSize(); }
    void RemoveItemByTAG(u32 tag_value);
    void RemoveAll();

    virtual void Show(const Frect& parent_rect, const Fvector2& point);
    virtual void Hide();

    virtual void Update();
    virtual void Draw();

    CUIListBoxItem* GetClickedItem();

    void AutoUpdateSize();

protected:
    CUIListBox m_UIListWnd;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CUIPropertiesBox);

add_to_type_list(CUIPropertiesBox);
#undef script_type_list
#define script_type_list save_type_list(CUIPropertiesBox)
