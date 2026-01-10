#pragma once

class CUIDialogWnd;
class CUIDialogWndEx;
class CUIWindow;

class dlgItem
{
public:
    CUIWindow* wnd;
    bool enabled{true};

    dlgItem(CUIWindow* pWnd);

    [[nodiscard]] bool operator<(const dlgItem& itm) const;
    [[nodiscard]] bool operator==(const dlgItem& that) const;
};

class recvItem
{
public:
    enum
    {
        eCrosshair = 1 << 0,
        eIndicators = 1 << 1
    };

    CUIDialogWnd* m_item;
    Flags8 m_flags;

    recvItem(CUIDialogWnd* r);

    [[nodiscard]] bool operator==(const recvItem& that) const;
};

class CDialogHolder : public ISheduled, public pureFrame
{
    RTTI_DECLARE_TYPEINFO(CDialogHolder, ISheduled, pureFrame);

public:
    // dialogs
    xr_vector<recvItem> m_input_receivers;
    xr_vector<dlgItem> m_dialogsToRender;

protected:
    xr_vector<std::unique_ptr<CUIDialogWndEx>> script_menus;

public:
    void StartMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators);
    void StopMenu(CUIDialogWnd* pDialog);

    void SetMainInputReceiver(CUIDialogWnd* ir, bool _find_remove, const Flags8 flags = {});

protected:
    void DoRenderDialogs();
    void CleanInternals();

public:
    CDialogHolder();
    ~CDialogHolder() override;

    virtual shared_str shedule_Name() const { return shared_str("CDialogHolder"); }
    virtual void shedule_Update(u32 dt);
    virtual float shedule_Scale() const;
    virtual bool shedule_Needed() { return true; }

    // dialogs
    CUIDialogWnd* MainInputReceiver();
    virtual void StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators);
    void StartStopMenu_script(std::unique_ptr<CUIDialogWndEx>& pDialog, bool bDoHideIndicators);
    void AddDialogToRender(CUIWindow* pDialog);
    void RemoveDialogToRender(CUIWindow* pDialog);
    [[nodiscard]] tmc::task<void> OnFrame() override;
    virtual bool UseIndicators() { return true; }
};
XR_SOL_BASE_CLASSES(CDialogHolder);
