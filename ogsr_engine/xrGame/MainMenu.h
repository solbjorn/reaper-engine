#pragma once

#include "..\xr_3da\iinputreceiver.h"
#include "..\xr_3da\IGame_Persistent.h"
#include "UIDialogHolder.h"
#include "ui/UIWndCallback.h"
#include "ui_base.h"

class CUIWindow;
class CUIDialogWnd;
class CUICursor;
class CUIMessageBoxEx;

class CMainMenu : public IMainMenu, public IInputReceiver, public pureRender, public CDialogHolder, public CUIWndCallback, public CDeviceResetNotifier
{
    RTTI_DECLARE_TYPEINFO(CMainMenu, IMainMenu, IInputReceiver, pureRender, CDialogHolder, CUIWndCallback, CDeviceResetNotifier);

private:
    CUIDialogWnd* m_startDialog{};

    enum
    {
        flRestoreConsole = (1 << 0),
        flRestorePause = (1 << 1),
        flRestorePauseStr = (1 << 2),
        flActive = (1 << 3),
        flNeedChangeCapture = (1 << 4),
        flRestoreCursor = (1 << 5),
        flGameSaveScreenshot = (1 << 6),
        flNeedVidRestart = (1 << 7),
    };
    Flags16 m_Flags;
    bool languageChanged{};

    u32 m_screenshotFrame;
    string_path m_screenshot_name;

    void ReadTextureInfo();

    xr_vector<CUIWindow*> m_pp_draw_wnds;

    ref_sound CurrentSound;

public:
    enum EErrorDlg
    {
        SessionTerminate,
        ErrMax,
        ErrNoError = ErrMax,
    };

protected:
    EErrorDlg m_NeedErrDialog;
    u32 m_start_time;

    xr_vector<CUIMessageBoxEx*> m_pMB_ErrDlgs;

public:
    u32 m_deactivated_frame;

    CMainMenu();
    ~CMainMenu() override;

    virtual void DestroyInternal(bool bForce);
    tmc::task<void> Activate(bool bActive) override;
    virtual bool IsActive();
    virtual bool CanSkipSceneRendering();

    [[nodiscard]] bool IsLanguageChanged() const { return languageChanged; }
    void SetLanguageChanged(bool status) { languageChanged = status; }

    virtual void IR_OnMouseMove(int x, int y);
    virtual void IR_OnMouseStop(int, int);

    tmc::task<void> IR_OnKeyboardPress(xr::key_id dik) override;
    void IR_OnKeyboardRelease(xr::key_id dik) override;
    tmc::task<void> IR_OnKeyboardHold(xr::key_id dik) override;

    tmc::task<void> IR_OnMouseWheel(gsl::index direction) override;

    bool OnRenderPPUI_query();
    void OnRenderPPUI_main();
    void OnRenderPPUI_PP();

    tmc::task<void> OnRender() override;
    tmc::task<void> OnFrame() override;
    virtual void StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators);
    virtual bool UseIndicators() { return false; }

    void OnDeviceCreate();

    tmc::task<void> Screenshot(IRender_interface::ScreenshotMode mode = IRender_interface::SM_NORMAL, gsl::czstring name = nullptr);
    void RegisterPPDraw(CUIWindow* w);
    void UnregisterPPDraw(CUIWindow* w);

    void SetErrorDialog(EErrorDlg ErrDlg);
    EErrorDlg GetErrorDialogType() const { return m_NeedErrDialog; }
    void CheckForErrorDlg();
    void OnSessionTerminate(LPCSTR reason);
    void SetNeedVidRestart();
    tmc::task<void> OnDeviceReset() override;
    LPCSTR GetGSVer();

    void PlaySound(gsl::czstring path);

private:
    tmc::task<void> play_sound_async(std::array<std::byte, 16>& arg);
};
XR_SOL_BASE_CLASSES(CMainMenu);

extern CMainMenu* MainMenu();

// console_commands.cpp
namespace xr
{
[[nodiscard]] gsl::czstring GetLanguagesToken();
}
