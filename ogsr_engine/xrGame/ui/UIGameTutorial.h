#pragma once

#include "../xr_level_controller.h"

class CUIWindow;
class CUIStatic;
class CUISequenceItem;
class CUIXml;

class CUISequencer : public pureFrame, public pureRender, public IInputReceiver
{
    RTTI_DECLARE_TYPEINFO(CUISequencer, pureFrame, pureRender, IInputReceiver);

protected:
    CUIWindow* m_UIWindow{};
    xr_deque<CUISequenceItem*> m_items;
    bool m_bActive;
    bool m_bPlayEachItem;
    bool GrabInput();

public:
    IInputReceiver* m_pStoredInputReceiver{};

    CUISequencer();
    ~CUISequencer() override = default;

    void Start(LPCSTR tutor_name);
    void Stop();
    void Next();

    void Destroy(); // be careful

    virtual void OnFrame();
    virtual void OnRender();
    CUIWindow* MainWnd() { return m_UIWindow; }
    bool IsActive() { return m_bActive; }

    // IInputReceiver
    virtual void IR_OnMousePress(int btn);
    virtual void IR_OnMouseRelease(int btn);
    virtual void IR_OnMouseHold(int btn);
    virtual void IR_OnMouseMove(int x, int y);
    virtual void IR_OnMouseStop(int x, int y);

    virtual void IR_OnKeyboardPress(int dik);
    virtual void IR_OnKeyboardRelease(int dik);
    virtual void IR_OnKeyboardHold(int dik);

    virtual void IR_OnMouseWheel(int direction);
    virtual void IR_OnActivate(void);
};

class XR_NOVTABLE CUISequenceItem : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CUISequenceItem);

public:
    xr_vector<int> m_disabled_actions;

protected:
    enum
    {
        etiNeedPauseOn = (1 << 0),
        etiNeedPauseOff = (1 << 1),
        etiStoredPauseState = (1 << 2),
        etiCanBeStopped = (1 << 3),
        etiGrabInput = (1 << 4),
        etiNeedPauseSound = (1 << 5),
        eti_last = 6
    };
    xr_vector<sol::function> m_start_lua_functions;
    xr_vector<sol::function> m_stop_lua_functions;

    Flags32 m_flags;
    CUISequencer* m_owner;

public:
    explicit CUISequenceItem(CUISequencer* owner) : m_owner(owner) { m_flags.zero(); }
    ~CUISequenceItem() override = 0;

    virtual void Load(CUIXml* xml, int idx) = 0;

    virtual void Start() = 0;
    virtual bool Stop(bool = false) = 0;

    virtual void Update() = 0;
    virtual void OnRender() = 0;
    virtual void OnKeyboardPress(int dik) = 0;

    virtual bool IsPlaying() = 0;

    bool AllowKey(int dik);
    bool GrabInput() { return !!m_flags.test(etiGrabInput); }
};

inline CUISequenceItem::~CUISequenceItem() = default;

class CUISequenceSimpleItem : public CUISequenceItem
{
    RTTI_DECLARE_TYPEINFO(CUISequenceSimpleItem, CUISequenceItem);

public:
    typedef CUISequenceItem inherited;

    struct SSubItem
    {
        CUIStatic* m_wnd;
        float m_start;
        float m_length;
        bool m_visible;

        void Start();
        void Stop();
    };

    DEFINE_VECTOR(SSubItem, SubItemVec, SubItemVecIt);
    SubItemVec m_subitems;

    CUIWindow* m_UIWindow;
    ref_sound m_sound;
    float m_time_start;
    float m_time_length;
    string64 m_pda_section;
    Fvector2 m_desired_cursor_pos;
    int m_continue_dik_guard;
    struct SActionItem
    {
        EGameActions m_action;
        shared_str m_functor;
        bool m_bfinalize;
    };
    xr_vector<SActionItem> m_actions;

    explicit CUISequenceSimpleItem(CUISequencer* owner) : CUISequenceItem{owner} {}
    ~CUISequenceSimpleItem() override;

    virtual void Load(CUIXml* xml, int idx);

    virtual void Start();
    virtual bool Stop(bool bForce = false);

    virtual void Update();
    virtual void OnRender() {}
    virtual void OnKeyboardPress(int dik);

    virtual bool IsPlaying();
};

class CUISequenceVideoItem : public CUISequenceItem
{
    RTTI_DECLARE_TYPEINFO(CUISequenceVideoItem, CUISequenceItem);

public:
    typedef CUISequenceItem inherited;
    ref_sound m_sound[2];
    FactoryPtr<IUISequenceVideoItem> m_texture;
    enum
    {
        etiPlaying = (1 << (eti_last + 0)),
        etiNeedStart = (1 << (eti_last + 1)),
        etiDelayed = (1 << (eti_last + 2)),
        etiBackVisible = (1 << (eti_last + 3)),
    };
    float m_delay{};
    CUIStatic* m_wnd{};
    u32 m_time_start{};
    u32 m_sync_time{};

    explicit CUISequenceVideoItem(CUISequencer* owner);
    ~CUISequenceVideoItem() override;

    virtual void Load(CUIXml* xml, int idx);

    virtual void Start();
    virtual bool Stop(bool bForce = false);

    virtual void Update();
    virtual void OnRender();
    void OnKeyboardPress(int) override {}

    virtual bool IsPlaying();
};
