//=============================================================================
//  Красивый текстовый баннер с множеством стилей анимации
//=============================================================================

#ifndef UI_TEXT_BANNER_H_
#define UI_TEXT_BANNER_H_

#include "UIStatic.h"

//-----------------------------------------------------------------------------/
//  Класс параметров эффекта
//-----------------------------------------------------------------------------/

struct EffectParams
{
    friend class CUITextBanner;

    float fPeriod{};
    bool bCyclic{true};
    bool bOn{true};
    int iAdditionalParam{};
    float fAdditionalParam{};
    int iEffectStage{};

    // Constructor
    EffectParams() = default;

private:
    float fTimePassed{};
};

//-----------------------------------------------------------------------------/
//  Класс анимироанного баннера
//-----------------------------------------------------------------------------/

class CUITextBanner : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CUITextBanner);

public:
    enum TextBannerStyles
    {
        tbsNone = 0,
        tbsFlicker = 1,
        tbsFade = 1 << 1
    };

    // Ctor and Dtor
    CUITextBanner() = default;
    ~CUITextBanner() override = default;

    virtual void Update();
    void XR_PRINTF(4, 5) Out(float x, float y, const char* fmt, ...);

    // Установить параметры визуализации баннера. Флаги см. перечисление TextBannerStyles
    EffectParams* SetStyleParams(const TextBannerStyles styleName);
    void ResetAnimation(const TextBannerStyles styleName);

    // Font
    void SetFont(CGameFont* pFont) { m_pFont = pFont; }
    CGameFont* GetFont() const { return m_pFont; }
    void SetFontSize(float sz) { fontSize = sz; }
    void SetFontAlignment(CGameFont::EAligment al) { aligment = al; }

    // Color
    void SetTextColor(u32 cl);
    u32 GetTextColor();

    // Вкл/выкл анимации
    void PlayAnimation() { m_bAnimate = true; }
    void StopAnimation() { m_bAnimate = false; }

protected:
    // Переменные времени для каждого из стилей.
    // В паре:	first	- контрольный период эффекта (задаваемый пользователем)
    //			second	- прошедшее время с текущего апдейта
    typedef xr_map<TextBannerStyles, EffectParams> StyleParams;
    typedef StyleParams::iterator StyleParams_it;
    StyleParams m_StyleParams;

    // Процедуры изменения параметров надписи для эффектов
    void EffectFade();
    void EffectFlicker();

    // Флаг, который определяет состояние анимации
    bool m_bAnimate{true};

    // Font
    CGameFont* m_pFont{};
    float fontSize{-1.f};
    CGameFont::EAligment aligment{CGameFont::alLeft};

    // Letters color
    u32 m_Cl{std::numeric_limits<u32>::max()};
};

#endif // UI_TEXT_BANNER_H_
