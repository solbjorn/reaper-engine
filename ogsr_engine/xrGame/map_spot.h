#pragma once
#include "ui/UIStatic.h"

class CMapLocation;
class CUIXml;

class CMapSpot : public CUIStatic
{
    RTTI_DECLARE_TYPEINFO(CMapSpot, CUIStatic);

public:
    typedef CUIStatic inherited;
    CMapLocation* m_map_location;

    bool m_bScale;
    Fvector2 m_originSize;

    explicit CMapSpot(CMapLocation*);
    ~CMapSpot() override;

    virtual void Load(CUIXml* xml, gsl::czstring path);
    CMapLocation* MapLocation() { return m_map_location; }
    [[nodiscard]] virtual gsl::czstring GetHint();
    void Update() override;
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button mouse_btn) override;
    void OnFocusLost() override;
};

class CMapSpotPointer : public CMapSpot
{
    RTTI_DECLARE_TYPEINFO(CMapSpotPointer, CMapSpot);

public:
    typedef CMapSpot inherited;
    xr_string m_pointer_hint;

    explicit CMapSpotPointer(CMapLocation*);
    ~CMapSpotPointer() override;

    [[nodiscard]] gsl::czstring GetHint() override;
};

class CMiniMapSpot : public CMapSpot
{
    RTTI_DECLARE_TYPEINFO(CMiniMapSpot, CMapSpot);

public:
    typedef CMapSpot inherited;
    ui_shader m_icon_above, m_icon_normal, m_icon_below;
    Frect m_tex_rect_above, m_tex_rect_normal, m_tex_rect_below;

    explicit CMiniMapSpot(CMapLocation*);
    ~CMiniMapSpot() override;

    void Load(CUIXml* xml, gsl::czstring path) override;
    void Draw() override;
};
