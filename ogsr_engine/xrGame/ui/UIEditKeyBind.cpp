#include "stdafx.h"

#include "UIEditKeyBind.h"

#include "UIColorAnimatorWrapper.h"

#include "../object_broker.h"
#include "../xr_level_controller.h"

#include "../../xr_3da/XR_IOConsole.h"

CUIEditKeyBind::CUIEditKeyBind(bool bPrim) : m_bPrimary{bPrim}
{
    m_pAnimation = xr_new<CUIColorAnimatorWrapper>(shared_str{"ui_map_area_anim"});
    m_pAnimation->Cyclic(true);
    m_bChanged = false;
    m_lines.SetTextComplexMode(false);
}

CUIEditKeyBind::~CUIEditKeyBind() { delete_data(m_pAnimation); }

namespace
{
[[nodiscard]] u32 cut_string_by_length(CGameFont* pFont, LPCSTR src, LPSTR dst, u32 dst_size, f32 length)
{
    if (pFont->IsMultibyte())
    {
        u16 nPos = pFont->GetCutLengthPos(length, src);
        VERIFY(nPos < dst_size);
        strncpy(dst, src, nPos);
        dst[nPos] = '\0';

        return nPos;
    }
    else
    {
        f32 text_len = pFont->SizeOf_(src);
        UI()->ClientToScreenScaledWidth(text_len);
        VERIFY(xr_strlen(src) <= dst_size);
        strcpy(dst, src);

        while (text_len > length)
        {
            dst[xr_strlen(dst) - 1] = 0;
            VERIFY(xr_strlen(dst));
            text_len = pFont->SizeOf_(dst);
            UI()->ClientToScreenScaledWidth(text_len);
        }

        return xr_strlen(dst);
    }
}
} // namespace

void CUIEditKeyBind::SetText(const char* text)
{
    if (text == nullptr || xr_strlen(text) == 0)
    {
        CUILabel::SetText("---");
    }
    else
    {
        string256 buff;
        std::ignore = cut_string_by_length(CUILinesOwner::GetFont(), text, buff, sizeof(buff), GetWidth());
        CUILabel::SetText(buff);
    }
}

void CUIEditKeyBind::Init(float x, float y, float width, float height)
{
    CUILabel::Init(x, y, width, height);
    InitTexture("ui_options_string");
}

void CUIEditKeyBind::InitTexture(LPCSTR texture, bool bHorizontal) { CUILabel::InitTexture(texture, bHorizontal); }

void CUIEditKeyBind::OnFocusLost()
{
    CUILabel::OnFocusLost();
    m_bEditMode = false;
    m_lines.SetTextColor((subst_alpha(m_lines.GetTextColor(), color_get_A(0xffffffff))));
}

bool CUIEditKeyBind::OnMouseDown(sf::Mouse::Button mouse_btn)
{
    if (m_bEditMode)
    {
        string64 message;

        m_keyboard = dik_to_ptr(xr::key_id{mouse_btn}, true);
        if (!m_keyboard)
            return true;

        SetText(m_keyboard->key_local_name.c_str());
        OnFocusLost();
        m_bChanged = true;

        strcpy_s(message, m_action->action_name);
        strcat_s(message, "=");
        strcat_s(message, m_keyboard->key_name);
        SendMessage2Group("key_binding", message);

        return true;
    }

    if (mouse_btn == sf::Mouse::Button::Left)
        m_bEditMode = m_bCursorOverWindow;

    return CUILabel::OnMouseDown(mouse_btn);
}

bool CUIEditKeyBind::OnKeyboard(xr::key_id dik, EUIMessages keyboard_action)
{
    if (dik.is<sf::Mouse::Button>())
        return false;

    if (CUILabel::OnKeyboard(dik, keyboard_action))
        return true;

    string64 message;
    if (m_bEditMode)
    {
        m_keyboard = dik_to_ptr(dik, true);
        if (!m_keyboard)
            return true;

        strcpy_s(message, m_action->action_name);
        strcat_s(message, "=");
        strcat_s(message, m_keyboard->key_name);
        SetText(m_keyboard->key_local_name.c_str());

        OnFocusLost();
        m_bChanged = true;
        SendMessage2Group("key_binding", message);

        return true;
    }

    return false;
}

void CUIEditKeyBind::Update()
{
    CUILabel::Update();

    m_bTextureAvailable = m_bCursorOverWindow;
    if (m_bEditMode)
    {
        m_pAnimation->Update();
        m_lines.SetTextColor((subst_alpha(m_lines.GetTextColor(), color_get_A(m_pAnimation->GetColor()))));
    }
}

void CUIEditKeyBind::Register(const char* entry, const char* group)
{
    CUIOptionsItem::Register(entry, group);
    m_action = action_name_to_ptr(entry);
    ASSERT_FMT(m_action, "Action [%s] not found. Group: [%s]", entry, group);
}

void CUIEditKeyBind::SetCurrentValue()
{
    const auto pbinding = &g_key_bindings[std::to_underlying(xr::action_id(*m_action))];

    int idx = (m_bPrimary) ? 0 : 1;
    m_keyboard = pbinding->m_keyboard[idx];

    if (m_keyboard)
        SetText(m_keyboard->key_local_name.c_str());
    else
        SetText(nullptr);
}

void CUIEditKeyBind::SaveValue()
{
    CUIOptionsItem::SaveValue();

    BindAction2Key();
    m_bChanged = false;
}

void CUIEditKeyBind::BindAction2Key()
{
    xr_string comm_unbind = (m_bPrimary) ? "unbind " : "unbind_sec ";
    comm_unbind += m_action->action_name;
    Console->Execute(comm_unbind.c_str());

    if (m_keyboard)
    {
        xr_string comm_bind = (m_bPrimary) ? "bind " : "bind_sec ";
        comm_bind += m_action->action_name;
        comm_bind += " ";
        comm_bind += m_keyboard->key_name;
        Console->Execute(comm_bind.c_str());
    }
}

bool CUIEditKeyBind::IsChanged() { return m_bChanged; }

void CUIEditKeyBind::OnMessage(const char* message)
{
    // message = "command=key"
    int eq = (int)strcspn(message, "=");

    if (!m_keyboard)
        return;

    if (std::is_neq(xr_strcmp(m_keyboard->key_name, message + eq + 1)))
        return;

    string64 command;
    strcpy_s(command, message);
    command[eq] = 0;

    if (std::is_eq(xr_strcmp(m_action->action_name, command)))
        return; // fuck

    SetText("---");
    m_keyboard = nullptr;
}
