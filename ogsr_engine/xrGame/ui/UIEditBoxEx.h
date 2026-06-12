#pragma once

#include "UICustomEdit.h"

class CUIFrameWindow;

class CUIEditBoxEx : public CUIMultiTextureOwner, public CUICustomEdit
{
    RTTI_DECLARE_TYPEINFO(CUIEditBoxEx, CUIMultiTextureOwner, CUICustomEdit);

public:
    CUIEditBoxEx();
    ~CUIEditBoxEx() override;

    // CUIWindow
    void Init(f32 x, f32 y, f32 width, f32 heigt) override;

    // CUIMultiTextureOwner
    void InitTexture(gsl::czstring texture) override;

protected:
    CUIFrameWindow* m_pFrameWindow;
};
XR_SOL_BASE_CLASSES(CUIEditBoxEx);
