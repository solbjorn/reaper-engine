#pragma once

#include "UIDragDropListEx.h"

class CUIOutfitDragDropList : public CUIDragDropListEx
{
    RTTI_DECLARE_TYPEINFO(CUIOutfitDragDropList, CUIDragDropListEx);

private:
    typedef CUIDragDropListEx inherited;
    CUIStatic* m_background;
    shared_str m_default_outfit;
    void SetOutfit(CUICellItem* itm);

public:
    CUIOutfitDragDropList();
    ~CUIOutfitDragDropList() override;

    void SetItem(CUICellItem* itm) override; // auto
    void SetItem(CUICellItem* itm, Fvector2 abs_pos) override; // start at cursor pos
    void SetItem(CUICellItem* itm, Ivector2 cell_pos) override; // start at cell
    [[nodiscard]] CUICellItem* RemoveItem(CUICellItem* itm, bool force_root) override;
    void Draw() override;
    void SetDefaultOutfit(LPCSTR default_outfit);
};
