//=============================================================================
//  Filename:   UIGameLog.h
//	Created by Vitaly 'Mad Max' Maximov, mad-max@gsc-game.kiev.ua
//	Copyright 2005. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer game log window
//=============================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
#include "UIScrollView.h"

class CUIXml;
class CUIPdaMsgListItem;
class CUIStatic;

class CUIGameLog : public CUIScrollView
{
    RTTI_DECLARE_TYPEINFO(CUIGameLog, CUIScrollView);

public:
    CUIGameLog();
    ~CUIGameLog() override;

    CUIPdaMsgListItem* AddPdaMessage(LPCSTR msg, float delay);
    void Update() override;

    void SetTextAtrib(CGameFont* pFont, u32 color);
    u32 GetTextColor();

private:
    xr_vector<CUIWindow*> toDelList;
    float kill_msg_height;
    u32 txt_color;
};
