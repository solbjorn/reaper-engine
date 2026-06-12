#pragma once

#include "UILabel.h"

class CUIListBoxItem : public CUILabel, public CUISelectable
{
    RTTI_DECLARE_TYPEINFO(CUIListBoxItem, CUILabel, CUISelectable);

public:
    using CUILabel::SetTextColor;

    CUIListBoxItem();
    ~CUIListBoxItem() override;

    void SetSelected(bool b) override;
    void Draw() override;
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button mouse_btn) override;
    [[nodiscard]] bool OnDbClick() override;
    void OnFocusReceive() override;
    [[nodiscard]] CGameFont* GetFont() override;
    void SetTextColor(u32 color, u32 color_s);
    void InitDefault();
    void SetTAG(u32 value);
    u32 GetTAG();

    void SetData(void* data);
    void* GetData();

    CUIStatic* AddField(LPCSTR txt, float len, LPCSTR key = "");
    LPCSTR GetField(LPCSTR key);

protected:
    float FieldsLength();
    xr_vector<CUIStatic*> fields;
    u32 txt_color;
    u32 txt_color_s;
    u32 tag;
    void* pData{};
    //.static	u32	uid_counter;
};
