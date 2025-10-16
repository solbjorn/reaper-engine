#pragma once

#include "object_destroyer.h"
#include "script_export_space.h"

// refs
class CUI;
class game_cl_GameState;
class CUIDialogWnd;
class CUICaption;
class CUIStatic;
class CUIWindow;
class CUIXml;

struct SDrawStaticStruct
{
    SDrawStaticStruct();
    void destroy();
    CUIStatic* m_static;
    float m_endTime;
    int m_priority{};
    shared_str m_name;
    void Draw();
    void Update();
    CUIStatic* wnd() { return m_static; }
    bool IsActual();
    bool operator==(LPCSTR str) { return (m_name == str); }
};

template <>
struct object_destroyer::default_destroy<SDrawStaticStruct>
{
    void operator()(SDrawStaticStruct& data) const { data.destroy(); }
};

using st_vec = xr_vector<SDrawStaticStruct>;

class CUIGameCustom : public DLL_Pure, public ISheduled
{
    RTTI_DECLARE_TYPEINFO(CUIGameCustom, DLL_Pure, ISheduled);

public:
    typedef ISheduled inherited;

protected:
    u32 uFlags;

    void SetFlag(u32 mask, BOOL flag)
    {
        if (flag)
            uFlags |= mask;
        else
            uFlags &= ~mask;
    }
    void InvertFlag(u32 mask)
    {
        if (uFlags & mask)
            uFlags &= ~mask;
        else
            uFlags |= mask;
    }
    BOOL GetFlag(u32 mask) { return uFlags & mask; }
    CUICaption* GameCaptions() { return m_pgameCaptions; }
    CUICaption* m_pgameCaptions;
    CUIXml* m_msgs_xml;
    void UpdateStaticPriorityIndex();

public:
    st_vec m_custom_statics;
    xr_vector<st_vec::size_type> m_custom_statics_sorted;

    virtual void SetClGame(game_cl_GameState*) {}
    virtual float shedule_Scale() const;
    virtual void shedule_Update(u32 dt);

    CUIGameCustom();
    virtual ~CUIGameCustom();

    virtual void Init() {}

    virtual void Render();
    virtual void OnFrame();
    virtual void reset_ui();

    virtual bool IR_OnKeyboardPress(int);
    virtual bool IR_OnKeyboardRelease(int);
    virtual bool IR_OnMouseMove(int, int);
    virtual bool IR_OnMouseWheel(int);

    void AddDialogToRender(CUIWindow* pDialog);
    void RemoveDialogToRender(CUIWindow* pDialog);

    CUIDialogWnd* MainInputReceiver();
    virtual void ReInitShownUI() = 0;
    virtual void HideShownDialogs() {}

    void AddCustomMessage(LPCSTR id, float x, float y, float font_size, CGameFont* pFont, u16 alignment, u32 color);
    void AddCustomMessage(LPCSTR id, float x, float y, float font_size, CGameFont* pFont, u16 alignment, u32 color, float flicker);
    void CustomMessageOut(LPCSTR id, LPCSTR msg, u32 color);
    void RemoveCustomMessage(LPCSTR id);

    SDrawStaticStruct* AddCustomStatic(LPCSTR id, bool bSingleInstance);
    SDrawStaticStruct* GetCustomStatic(LPCSTR id);
    void RemoveCustomStatic(LPCSTR id);

    virtual shared_str shedule_Name() const { return shared_str("CUIGameCustom"); }
    virtual bool shedule_Needed() { return true; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CUIGameCustom);
#undef script_type_list
#define script_type_list save_type_list(CUIGameCustom)
