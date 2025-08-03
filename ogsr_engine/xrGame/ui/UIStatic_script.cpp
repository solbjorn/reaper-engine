#include "stdafx.h"

#include "Level.h"
#include "UIStatic.h"
#include "UILines.h"
#include "UIMap.h"

static void UIMiniMapZoom(CUIMiniMap* wnd, float scale)
{
    Fvector2 wnd_size;
    float zoom_factor = float(wnd->GetParent()->GetWndRect().width()) / 100.0f;
    wnd_size.x = wnd->BoundRect().width() * zoom_factor * scale /*m_fScale*/;
    wnd_size.y = wnd->BoundRect().height() * zoom_factor * scale /*m_fScale*/;
    wnd->SetWndSize(wnd_size);
}

static void UIMiniMapInit(CUIMiniMap* wnd)
{
    CUIWindow* parent = wnd->GetParent();

    CInifile* pLtx = pGameIni;

    if (!pLtx->section_exist(Level().name()))
        pLtx = Level().pLevel;

    wnd->Init(Level().name(), *pLtx, "hud\\default");

    Frect r;
    parent->GetAbsoluteRect(r);
    wnd->SetClipRect(r);
    wnd->WorkingArea().set(r);

    UIMiniMapZoom(wnd, 1.f);
}

void CUIStatic::script_register(sol::state_view& lua)
{
    lua.new_usertype<CUIStatic>(
        "CUIStatic", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CUIStatic>), "SetText", sol::resolve<void(LPCSTR)>(&CUIStatic::SetText),
        "SetTextST", sol::resolve<void(LPCSTR)>(&CUIStatic::SetTextST), "GetText", &CUIStatic::GetText, "IsMultibyteFont",
        [](CUIStatic* self) -> bool { return self->m_pLines->GetFont()->IsMultibyte(); }, "SetTextX", &CUIStatic::SetTextX, "SetTextY", &CUIStatic::SetTextY, "GetTextX",
        &CUIStatic::GetTextX, "GetTextY", &CUIStatic::GetTextY, "SetColor", &CUIStatic::SetColor, "GetColor", &CUIStatic::GetColor, "SetColorA",
        [](CUIStatic* self, u8 alpha) { self->SetColor(subst_alpha(self->GetColor(), alpha)); }, "SetTextColor", &CUIStatic::SetTextColor_script, "Init",
        sol::overload(sol::resolve<void(float, float, float, float)>(&CUIStatic::Init), sol::resolve<void(LPCSTR, float, float, float, float)>(&CUIStatic::Init)), "InitTexture",
        &CUIStatic::InitTexture, "SetTextureOffset", &CUIStatic::SetTextureOffset, "GetOriginalRect", &CUIStatic::GetOriginalRect, "SetOriginalRect",
        sol::resolve<void(float, float, float, float)>(&CUIStatic::SetOriginalRect), "ResetOriginalRect", &CUIStatic::ResetOriginalRect, "SetNoShaderCache",
        &CUIStatic::SetNoShaderCache, "SetStretchTexture", &CUIStatic::SetStretchTexture, "GetStretchTexture", &CUIStatic::GetStretchTexture, "SetTextAlign",
        &CUIStatic::SetTextAlign_script, "GetTextAlign", &CUIStatic::GetTextAlign_script, "SetHeading", &CUIStatic::SetHeading, "GetHeading", &CUIStatic::GetHeading,
        "EnableHeading", &CUIStatic::EnableHeading, "ClipperOn", &CUIStatic::ClipperOn, "ClipperOff", sol::resolve<void()>(&CUIStatic::ClipperOff), "GetClipperState",
        &CUIStatic::GetClipperState, "SetClipRect", &CUIStatic::SetClipRect, "GetClipRect", &CUIStatic::GetClipperRect, "SetTextComplexMode", &CUIStatic::SetTextComplexMode,
        "AdjustWidthToText", &CUIStatic::AdjustWidthToText, "AdjustHeightToText", &CUIStatic::AdjustHeightToText, "SetVTextAlign", &CUIStatic::SetVTextAlignment, "SetTextPos",
        &CUIStatic::SetTextPos, sol::base_classes, xr_sol_bases<CUIStatic>());

    lua.new_usertype<CUIMiniMap>("CUIMiniMap", sol::no_constructor, sol::call_constructor, sol::constructors<CUIMiniMap()>(), "SetRounded", &CUIMiniMap::SetRounded, "SetLocked",
                                 &CUIMiniMap::SetLocked, "Init", &UIMiniMapInit, "Zoom", &UIMiniMapZoom, "SetActivePoint", &CUIMiniMap::SetActivePoint, sol::base_classes,
                                 xr_sol_bases<CUIMiniMap>());
}
