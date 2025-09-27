//////////////////////////////////////////////////////////////////////
// UIListItem.cpp: элемент окна списка CListWnd
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIlistitem.h"

CUIListItem::CUIListItem()
{
    m_eButtonState = BUTTON_NORMAL;
    m_ePressMode = NORMAL_PRESS;

    m_bButtonClicked = false;

    SetAutoDelete(true);
    SetTextAlignment(CGameFont::alLeft);
}

CUIListItem::~CUIListItem() {}

void CUIListItem::Init(float x, float y, float width, float height)
{
    inherited::Init(x, y, width, height);
    SetPressMode(CUIButton::DOWN_PRESS);
    SetPushOffset(Fvector2().set(0.0f, 0.0f));
}

void CUIListItem::InitTexture(LPCSTR tex_name)
{
    CUIButton::InitTexture(tex_name);
    SetTextX(m_UIStaticItem.GetRect().width());
}

void CUIListItem::Init(const char* str, float x, float y, float width, float height)
{
    Init(x, y, width, height);
    SetTextST(str);
}

bool CUIListItem::IsHighlightText() { return CUIButton::IsHighlightText(); }
