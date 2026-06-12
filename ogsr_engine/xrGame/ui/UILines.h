// File:		UILines.h
// Description:	Multilines Text Control
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UILine.h"
#include "../UI.h"
#include "uiabstract.h"

class CUILines : public IUITextControl, public CUISimpleWindow
{
    RTTI_DECLARE_TYPEINFO(CUILines, IUITextControl, CUISimpleWindow);

public:
    friend class CUICustomEdit;

    CUILines();
    ~CUILines() override;

    // IUITextControl methods
    void SetText(gsl::czstring text) override;
    [[nodiscard]] gsl::czstring GetText() override;
    void SetTextColor(u32 color) override;
    [[nodiscard]] u32 GetTextColor() override { return m_dwTextColor; }
    [[nodiscard]] u32& GetTextColorRef() { return m_dwTextColor; }
    void SetFont(CGameFont* pFont) override;
    [[nodiscard]] CGameFont* GetFont() override { return m_pFont; }
    void SetTextAlignment(ETextAlignment al) override { m_eTextAlign = al; }
    [[nodiscard]] ETextAlignment GetTextAlignment() override { return m_eTextAlign; }
    void SetVTextAlignment(EVTextAlignment al) { m_eVTextAlign = al; }
    [[nodiscard]] EVTextAlignment GetVTextAlignment() { return m_eVTextAlign; }

    // additional
    void SetCursorColor(u32 color) { m_dwCursorColor = color; }
    void AddCharAtCursor(const u16 ch);
    void DelLeftChar();
    void DelCurrentChar();
    void MoveCursorToEnd();

    void SetTextComplexMode(bool mode = true);
    bool GetTextComplexMode() const;
    void SetPasswordMode(bool mode = true);
    void SetColoringMode(bool mode);
    void SetUseNewLineMode(bool mode);

    // IUISimpleWindow methods
    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void Draw() override;
    void Draw(f32 x, f32 y) override;
    void Update() override;
    IC void SetWndSize_inline(const Fvector2& wnd_size);

    // own methods
    void Reset();
    void ParseText();
    float GetVisibleHeight();

    // cursor control
    int m_iCursorPos{};
    void IncCursorPos();
    void DecCursorPos();

protected:
    Ivector2 m_cursor_pos{};
    void UpdateCursor();
    // %c[255,255,255,255]
    [[nodiscard]] u32 GetColorFromText(std::string_view str) const;
    float GetIndentByAlign() const;
    float GetVIndentByAlign();
    void CutFirstColoredTextEntry(xr_string& entry, u32& color, xr_string& text) const;
    CUILine* ParseTextToColoredLine(const xr_string& str);
    void DrawCursor(float x, float y);

    // IUITextControl data
    typedef xr_string Text;
    typedef xr_vector<CUILine> LinesVector;
    typedef LinesVector::iterator LinesVector_it;
    LinesVector m_lines; // parsed text
    float m_interval{}; // interval
    bool m_bDrawCursor{};

    Text m_text;
    xr_vector<bool> m_text_mask;

    ETextAlignment m_eTextAlign;
    EVTextAlignment m_eVTextAlign;
    u32 m_dwTextColor;
    u32 m_dwCursorColor;

    CGameFont* m_pFont;

    enum
    {
        flNeedReparse = (1 << 0),
        flComplexMode = (1 << 1),
        flPasswordMode = (1 << 2),
        flColoringMode = (1 << 3),
        flRecognizeNewLine = (1 << 5)
    };

private:
    Flags8 uFlags;
    float m_oldWidth{};
};

class CUILinesOwner : public IUITextControl
{
    RTTI_DECLARE_TYPEINFO(CUILinesOwner, IUITextControl);

public:
    ~CUILinesOwner() override = default;

    // IUIFontControl{
    void SetTextColor(u32 color) override { m_lines.SetTextColor(color); }
    [[nodiscard]] u32 GetTextColor() override { return m_lines.GetTextColor(); }
    void SetFont(CGameFont* pFont) override { m_lines.SetFont(pFont); }
    [[nodiscard]] CGameFont* GetFont() override { return m_lines.GetFont(); }
    void SetTextAlignment(ETextAlignment alignment) override { m_lines.SetTextAlignment(alignment); }
    [[nodiscard]] ETextAlignment GetTextAlignment() override { return m_lines.GetTextAlignment(); }

    // IUITextControl : public IUIFontControl{
    void SetText(gsl::czstring text) override { m_lines.SetText(text); }
    [[nodiscard]] gsl::czstring GetText() override { return m_lines.GetText(); }

    // own
    virtual void SetTextPosX(f32 x) { m_textPos.x = x; }
    virtual void SetTextPosY(f32 y) { m_textPos.y = y; }

protected:
    Fvector2 m_textPos{};
    CUILines m_lines;
};
