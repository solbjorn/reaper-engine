//=============================================================================
//  Filename:   UIMultiTextStatic.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Статик контрол на котором можно выводить множество надписей различными
//	шрифтами, цветами и даже с анимацией
//=============================================================================

#ifndef UI_MULTITEXT_STATIC_H_
#define UI_MULTITEXT_STATIC_H_

#include "UITextBanner.h"

class CUIMultiTextStatic : public CUIStatic
{
    RTTI_DECLARE_TYPEINFO(CUIMultiTextStatic, CUIStatic);

public:
    typedef CUIStatic inherited;

public:
    typedef struct SPh
    {
        float outX{};
        float outY{};
        float maxWidth{-1.0f};

        CUIStatic::EElipsisPosition elipsisPos{CUIStatic::eepEnd};
        CUITextBanner effect;

        shared_str str;
        shared_str key;

        // Ctor
        SPh();

        void XR_PRINTF(2, 3) SetText(const char* fmt, ...);
    } SinglePhrase;

    using Phrases = xr_vector<std::unique_ptr<SinglePhrase>>;

protected:
    Phrases m_vPhrases;

public:
    CUIMultiTextStatic();
    ~CUIMultiTextStatic() override;

    virtual void Draw();
    virtual void Update();
    // Добавить надпись
    // Return:	Указатель на добавленную запись
    SinglePhrase* AddPhrase();
    // Получить запись по номеру
    SinglePhrase* GetPhraseByIndex(u32 idx);
    void RemovePhraseByIndex(u32 idx);
};

class CUICaption : public virtual RTTI::Enable, protected CUIMultiTextStatic
{
    RTTI_DECLARE_TYPEINFO(CUICaption, CUIMultiTextStatic);

private:
    typedef CUIMultiTextStatic inherited;

    u32 findIndexOf(const shared_str& key) const;
    u32 findIndexOf_(const shared_str& key) const;

public:
    ~CUICaption() override = default;

    virtual void Draw();
    void addCustomMessage(const shared_str& msg_name, float x, float y, float font_size, CGameFont* pFont, CGameFont::EAligment al, u32 color, LPCSTR def_str = "");
    EffectParams* customizeMessage(const shared_str& msg_name, const CUITextBanner::TextBannerStyles styleName);
    void setCaption(const shared_str& msg_name, LPCSTR message_to_out, u32 color = 0, bool replaceColor = false);
    void removeCustomMessage(const shared_str& msg_name);
};

#endif // UI_MULTITEXT_STATIC_H_
