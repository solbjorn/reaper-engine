#pragma once

#include "UICursor.h"
#include "UIDialogHolder.h"

// refs
class CHUDManager;
class CWeapon;
class CUIGameCustom;
class CUIMainIngameWnd;
class CUIMessagesWindow;
struct SDrawStaticStruct;

class CUI : public CDialogHolder
{
    RTTI_DECLARE_TYPEINFO(CUI, CDialogHolder);

public:
    CUIGameCustom* pUIGame;
    bool m_bShowGameIndicators;

    CHUDManager* m_Parent;
    CUIMainIngameWnd* UIMainIngameWnd;
    CUIMessagesWindow* m_pMessagesWnd;
    bool hud_disabled_by_user{};

public:
    CUI(CHUDManager* p);
    virtual ~CUI();

    bool Render();
    void UIOnFrame();

    void Load(CUIGameCustom* pGameUI);
    void UnLoad();

    bool IR_OnKeyboardHold(int dik);
    bool IR_OnKeyboardPress(int dik);
    bool IR_OnKeyboardRelease(int dik);
    bool IR_OnMouseMove(int, int);
    bool IR_OnMouseWheel(int direction);

    CUIGameCustom* UIGame() { return pUIGame; }

    void ShowGameIndicators();
    void HideGameIndicators();
    bool GameIndicatorsShown() { return m_bShowGameIndicators; };

    void ShowCrosshair();
    void HideCrosshair();
    bool CrosshairShown();

    SDrawStaticStruct* AddInfoMessage(LPCSTR message);
    void OnConnected();
};
