#pragma once

#include "UICellItem.h"
#include "../Weapon.h"

class CUIInventoryCellItem : public CUICellItem
{
    RTTI_DECLARE_TYPEINFO(CUIInventoryCellItem, CUICellItem);

public:
    typedef CUICellItem inherited;

protected:
    bool b_auto_drag_childs;

public:
    explicit CUIInventoryCellItem(CInventoryItem* itm);
    // Real Wolf: Для метода get_cell_item(). 25.07.2014.
    ~CUIInventoryCellItem() override;

    void Update() override;
    [[nodiscard]] bool EqualTo(CUICellItem* itm) override;
    [[nodiscard]] CUIDragItem* CreateDragItem() override;
    [[nodiscard]] CInventoryItem* object() { return static_cast<CInventoryItem*>(m_pData); }

    // Real Wolf: Для коллбеков. 25.07.2014.
    void OnFocusReceive() override;
    void OnFocusLost() override;
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages action) override;
};

class CUIAmmoCellItem : public CUIInventoryCellItem
{
    RTTI_DECLARE_TYPEINFO(CUIAmmoCellItem, CUIInventoryCellItem);

private:
    using inherited = CUIInventoryCellItem;

public:
    explicit CUIAmmoCellItem(CWeaponAmmo* itm);
    ~CUIAmmoCellItem() override = default;

    void Update() override;
    void UpdateItemText() override;
    [[nodiscard]] bool EqualTo(CUICellItem* itm) override;

    [[nodiscard]] CWeaponAmmo* object() { return static_cast<CWeaponAmmo*>(m_pData); }
};

class CUIWeaponCellItem : public CUIInventoryCellItem
{
    RTTI_DECLARE_TYPEINFO(CUIWeaponCellItem, CUIInventoryCellItem);

public:
    typedef CUIInventoryCellItem inherited;

    enum eAddonType
    {
        eSilencer = 0,
        eScope,
        eLauncher,
        eMaxAddon
    };
    CUIStatic* m_addons[eMaxAddon];

protected:
    Fvector2 m_addon_offset[eMaxAddon];

    void CreateIcon(eAddonType, CIconParams& params);
    void DestroyIcon(eAddonType);
    [[nodiscard]] CUIStatic* GetIcon(eAddonType);
    void InitAddon(CUIStatic* s, CIconParams& params, Fvector2 offset, bool b_rotate);
    void InitAllAddons(CUIStatic* s_silencer, CUIStatic* s_scope, CUIStatic* s_launcher, bool b_vertical);
    [[nodiscard]] bool is_scope();
    [[nodiscard]] bool is_silencer();
    [[nodiscard]] bool is_launcher();

public:
    explicit CUIWeaponCellItem(CWeapon* itm);
    ~CUIWeaponCellItem() override;

    void Update() override;
    [[nodiscard]] CWeapon* object() { return (CWeapon*)m_pData; }
    void OnAfterChild(CUIDragDropListEx* parent_list) override;
    [[nodiscard]] CUIDragItem* CreateDragItem() override;
    [[nodiscard]] bool EqualTo(CUICellItem* itm) override;
    [[nodiscard]] CUIStatic* get_addon_static(u32 idx) { return m_addons[idx]; }
    [[nodiscard]] Fvector2 get_addon_offset(u32 idx) { return m_addon_offset[idx]; }
};
