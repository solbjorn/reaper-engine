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
    CUIGameCustom* pUIGame{};
    CHUDManager* m_Parent;
    CUIMainIngameWnd* UIMainIngameWnd;
    CUIMessagesWindow* m_pMessagesWnd;

    bool m_bShowGameIndicators;
    bool hud_disabled_by_user{};

public:
    explicit CUI(CHUDManager* p);
    ~CUI() override;

    bool Render();
    void UIOnFrame();

    void Load(CUIGameCustom* pGameUI);
    void UnLoad();

    tmc::task<bool> IR_OnKeyboardHold(gsl::index dik);
    tmc::task<bool> IR_OnKeyboardPress(gsl::index dik);
    bool IR_OnKeyboardRelease(int dik);
    bool IR_OnMouseMove(int, int);
    tmc::task<bool> IR_OnMouseWheel(gsl::index direction);

    CUIGameCustom* UIGame() { return pUIGame; }

    void ShowGameIndicators();
    void HideGameIndicators();
    bool GameIndicatorsShown() { return m_bShowGameIndicators; }

    void ShowCrosshair();
    void HideCrosshair();
    bool CrosshairShown();

    SDrawStaticStruct* AddInfoMessage(LPCSTR message);
    void OnConnected();
};
