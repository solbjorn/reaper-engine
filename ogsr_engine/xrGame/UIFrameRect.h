#ifndef __XR_UIFRAMERECT_H__
#define __XR_UIFRAMERECT_H__

#include "uistaticitem.h"
#include "ui/uiabstract.h"

class CUIFrameRect : public CUISimpleWindow, CUIMultiTextureOwner // public CUICustomItem
{
    RTTI_DECLARE_TYPEINFO(CUIFrameRect, CUISimpleWindow, CUIMultiTextureOwner);

public:
    enum EFramePart
    {
        fmBK = 0,
        fmL,
        fmR,
        fmT,
        fmB,
        fmLT,
        fmRB,
        fmRT,
        fmLB,
        fmMax
    };

protected:
    CUIStaticItem frame[fmMax];

public:
    Flags16 m_itm_mask;

    friend class CUIFrameWindow;
    using CUISimpleWindow::Init;

    CUIFrameRect();
    ~CUIFrameRect() override;

    virtual void Init(gsl::czstring base_name, f32 x, f32 y, f32 w, f32 h);
    void InitTexture(gsl::czstring texture) override;
    void Draw() override;
    void Draw(f32 x, f32 y) override;
    void SetWndPos(f32 x, f32 y) override;
    void SetWndPos(const Fvector2& pos) override;
    void SetWndSize(const Fvector2& size) override;
    void SetWndRect(const Frect& rect) override;
    void SetWidth(f32 width) override;
    void SetHeight(f32 height) override;
    void Update() override;
    void SetTextureColor(u32 cl);
    void SetVisiblePart(EFramePart p, BOOL b) { m_itm_mask.set(u16(1 << p), b); }

protected:
    enum
    {
        flValidSize = (1 << 0),
        flSingleTex = (1 << 1),
    };
    Flags8 uFlags;

    void UpdateSize();
};

#endif //__XR_UIFRAMERECT_H__
