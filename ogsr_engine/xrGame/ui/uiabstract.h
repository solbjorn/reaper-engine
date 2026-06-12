#pragma once

#include "../ui_base.h"

typedef CGameFont::EAligment ETextAlignment;

class XR_NOVTABLE IUIFontControl : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUIFontControl);

public:
    ~IUIFontControl() override = 0;

    virtual void SetTextColor(u32 color) = 0;
    [[nodiscard]] virtual u32 GetTextColor() = 0;
    virtual void SetFont(CGameFont* pFont) = 0;
    [[nodiscard]] virtual CGameFont* GetFont() = 0;
    virtual void SetTextAlignment(ETextAlignment alignment) = 0;
    [[nodiscard]] virtual ETextAlignment GetTextAlignment() = 0;
};

inline IUIFontControl::~IUIFontControl() = default;

typedef enum
{
    valTop = 0,
    valCenter,
    valBotton
} EVTextAlignment;

class XR_NOVTABLE IUITextControl : public IUIFontControl
{
    RTTI_DECLARE_TYPEINFO(IUITextControl, IUIFontControl);

public:
    ~IUITextControl() override = 0;

    virtual void SetText(gsl::czstring text) = 0;
    [[nodiscard]] virtual gsl::czstring GetText() = 0;
};

inline IUITextControl::~IUITextControl() = default;

// Texture controls
class XR_NOVTABLE IUISimpleTextureControl : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUISimpleTextureControl);

public:
    ~IUISimpleTextureControl() override = 0;

    virtual void CreateShader(gsl::czstring tex, gsl::czstring sh = "hud\\default") = 0;
    virtual void SetShader(const ui_shader& sh) = 0;
    virtual void SetTextureColor(u32 color) = 0;
    [[nodiscard]] virtual u32 GetTextureColor() const = 0;
    virtual void SetOriginalRect(const Frect& r) = 0;
    virtual void SetOriginalRectEx(const Frect& r) = 0;
};

inline IUISimpleTextureControl::~IUISimpleTextureControl() = default;

class XR_NOVTABLE IUIMultiTextureOwner : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUIMultiTextureOwner);

public:
    ~IUIMultiTextureOwner() override = 0;

    virtual void InitTexture(gsl::czstring texture) = 0;
    [[nodiscard]] virtual bool GetTextureAvailability() = 0;
    [[nodiscard]] virtual bool GetTextureVisible() = 0;
};

inline IUIMultiTextureOwner::~IUIMultiTextureOwner() = default;

class CUIMultiTextureOwner : public IUIMultiTextureOwner
{
    RTTI_DECLARE_TYPEINFO(CUIMultiTextureOwner, IUIMultiTextureOwner);

public:
    CUIMultiTextureOwner() = default;
    ~CUIMultiTextureOwner() override = default;

    [[nodiscard]] bool GetTextureAvailability() override { return m_bTextureAvailable; }
    [[nodiscard]] bool GetTextureVisible() override { return m_bTextureVisible; }

protected:
    bool m_bTextureAvailable{};
    bool m_bTextureVisible{};
};

class XR_NOVTABLE IUISingleTextureOwner : public CUIMultiTextureOwner, public IUISimpleTextureControl
{
    RTTI_DECLARE_TYPEINFO(IUISingleTextureOwner, CUIMultiTextureOwner, IUISimpleTextureControl);

public:
    ~IUISingleTextureOwner() override = 0;

    virtual void InitTextureEx(gsl::czstring texture, gsl::czstring shader) = 0;
    virtual void SetStretchTexture(bool stretch) = 0;
    [[nodiscard]] virtual bool GetStretchTexture() = 0;
};

inline IUISingleTextureOwner::~IUISingleTextureOwner() = default;

class CUISingleTextureOwner : public IUISingleTextureOwner
{
    RTTI_DECLARE_TYPEINFO(CUISingleTextureOwner, IUISingleTextureOwner);

public:
    CUISingleTextureOwner() = default;
    ~CUISingleTextureOwner() override = default;

    void SetStretchTexture(bool stretch) override { m_bStretchTexture = stretch; }
    [[nodiscard]] bool GetStretchTexture() override { return m_bStretchTexture; }

protected:
    bool m_bStretchTexture;
};

// Window
enum EWindowAlignment
{
    waNone = 0,
    waLeft = 1,
    waRight = 2,
    waTop = 4,
    waBottom = 8,
    waCenter = 16
};

class XR_NOVTABLE IUISimpleWindow : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUISimpleWindow);

public:
    ~IUISimpleWindow() override = 0;

    virtual void Init(f32 x, f32 y, f32 width, f32 height) = 0;
    virtual void Draw() = 0;
    virtual void Draw(f32 x, f32 y) = 0;
    virtual void Update() = 0;
    virtual void SetWndPos(const Fvector2& pos) = 0;
    virtual void SetWndPos(f32 x, f32 y) = 0;
    virtual void SetWndSize(const Fvector2& size) = 0;
    virtual void SetWndRect(const Frect& rect) = 0;
    virtual void SetHeight(f32 height) = 0;
    virtual void SetWidth(f32 width) = 0;
};

inline IUISimpleWindow::~IUISimpleWindow() = default;

class CUISimpleWindow : public IUISimpleWindow
{
    RTTI_DECLARE_TYPEINFO(CUISimpleWindow, IUISimpleWindow);

public:
    CUISimpleWindow() = default;
    ~CUISimpleWindow() override = default;

    void Init(f32 x, f32 y, f32 width, f32 height) override
    {
        m_wndPos.set(x, y);
        m_wndSize.set(width, height);
    }

    void SetWndPos(const Fvector2& pos) override { m_wndPos.set(pos.x, pos.y); }
    void SetWndPos(f32 x, f32 y) override { m_wndPos.set(x, y); }
    IC Fvector2 GetWndPos() const { return m_wndPos; }
    void SetWndSize(const Fvector2& size) override { m_wndSize = size; }
    IC Fvector2 GetWndSize() const { return m_wndSize; }
    void SetHeight(f32 height) override { m_wndSize.y = height; }
    IC float GetHeight() const { return m_wndSize.y; }
    void SetWidth(f32 width) override { m_wndSize.x = width; }
    IC float GetWidth() const { return m_wndSize.x; }
    IC void SetVisible(bool vis) { m_bShowMe = vis; }
    IC bool GetVisible() const { return m_bShowMe; }
    IC void SetAlignment(EWindowAlignment al) { m_alignment = al; }

    virtual void SetWndRect(float x, float y, float width, float height)
    {
        m_wndPos.set(x, y);
        m_wndSize.set(width, height);
    }

    void SetWndRect(const Frect& rect) override { SetWndRect(rect.lt.x, rect.lt.y, rect.width(), rect.height()); }

    IC Frect GetWndRect() const
    {
        Frect r;
        GetWndRect(r);
        return r;
    }

    IC void GetWndRect(Frect& res) const
    {
        switch (m_alignment)
        {
        case waNone: res.set(m_wndPos.x, m_wndPos.y, m_wndPos.x + m_wndSize.x, m_wndPos.y + m_wndSize.y); break;
        case waCenter: {
            float half_w = m_wndSize.x / 2.0f;
            float half_h = m_wndSize.y / 2.0f;
            res.set(m_wndPos.x - half_w, m_wndPos.y - half_h, m_wndPos.x + half_w, m_wndPos.y + half_h);
        }
        break;
        default: NODEFAULT;
        }
    }

    void MoveWndDelta(float dx, float dy)
    {
        m_wndPos.x += dx;
        m_wndPos.y += dy;
    }

    void MoveWndDelta(const Fvector2& d) { MoveWndDelta(d.x, d.y); }

protected:
    Fvector2 m_wndPos{};
    Fvector2 m_wndSize{};
    EWindowAlignment m_alignment{waNone};
    bool m_bShowMe{};
};

class CUISelectable : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CUISelectable);

protected:
    bool m_bSelected{};

public:
    CUISelectable() = default;
    ~CUISelectable() override = default;

    bool GetSelected() const { return m_bSelected; }
    virtual void SetSelected(bool b) { m_bSelected = b; }
};
