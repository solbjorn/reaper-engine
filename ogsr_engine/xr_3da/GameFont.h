#pragma once

#include "MbHelpers.h"

#include "../Include/xrRender/FontRender.h"

class CGameFont
{
    friend class dxFontRender;

public:
    enum EAligment : u32
    {
        alLeft = 0,
        alRight,
        alCenter,
        alJustified
    };

private:
    struct String
    {
        string1024 string;
        float x, y;
        float height;
        u32 color;
        EAligment align;
    };

    float fXStep;

    float fTCHeight;

    float fXScale{};
    float fYScale{};

    IFontRender* pFontRender;

    std::unique_ptr<Fvector[]> TCMap;
    EAligment eCurrentAlignment;
    u32 dwCurrentColor;
    float fCurrentHeight;
    float fCurrentX, fCurrentY;
    Fvector2 vInterval;

protected:
    float fHeight; // оригинальная высота

    xr_vector<String> strings;

    Ivector2 vTS;

    u32 uFlags;

public:
    enum : u32
    {
        fsGradient = (1 << 0),
        fsDeviceIndependent = (1 << 1),
        fsValid = (1 << 2),
        fsMultibyte = (1 << 3),
    };

    IC const Fvector& GetCharTC(u16 c) const { return TCMap[c]; }

public:
    explicit CGameFont(LPCSTR section, u32 flags = 0);
    explicit CGameFont(LPCSTR shader, LPCSTR texture, const char* section, u32 flags = 0);
    ~CGameFont();

    void Initialize(LPCSTR shader, LPCSTR texture, const char* sect);

    IC void SetColor(u32 C) { dwCurrentColor = C; }

    void SetHeightI(float S);
    void SetHeight(float S);

    IC float GetHeight() const { return fCurrentHeight; }

    IC void SetInterval(float x, float y) { vInterval.set(x, y); }
    IC void SetInterval(const Fvector2& v) { vInterval.set(v); }

    IC const Fvector2& GetInterval() const { return vInterval; }

    IC void SetAligment(EAligment aligment) { eCurrentAlignment = aligment; }
    IC EAligment GetAligment() { return eCurrentAlignment; }

    float SizeOf_(LPCSTR s);
    float SizeOf_(const wide_char* wsStr);
    float SizeOf_(const char cChar); // only ANSII
    float SizeOf_(const u16 cChar); // only UTF-8

    float CurrentHeight_();

    float ScaleHeightDelta() const { return (fCurrentHeight * vInterval.y * GetHeightScale() - fCurrentHeight * vInterval.y) / 2; }

    void OutSetI(float x, float y);
    void OutSet(float x, float y);

    void XR_PRINTF(9, 0) MasterOut(BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip, float _x, float _y, float _skip, LPCSTR fmt, va_list p);

    BOOL IsMultibyte() const { return (uFlags & fsMultibyte); }

    u32 SmartLength(const char* S);

    u16 SplitByWidth(u16* puBuffer, u16 uBufferSize, float fTargetWidth, const char* pszText);
    u16 GetCutLengthPos(float fTargetWidth, const char* pszText);

    void XR_PRINTF(4, 5) OutI(float _x, float _y, LPCSTR fmt, ...);
    void XR_PRINTF(4, 5) Out(float _x, float _y, LPCSTR fmt, ...);
    void XR_PRINTF(2, 3) OutNext(LPCSTR fmt, ...);

    void OutSkip(float val = 1.f);

    void OnRender();

    IC void Clear() { strings.clear(); }

    float GetWidthScale() const;
    float GetHeightScale() const;

    void SetWidthScale(float f) { fXScale = f; }
    void SetHeightScale(float f) { fYScale = f; }

    float GetfXStep() const { return fXStep; }

    shared_str m_font_name;
    bool m_bCustom{};
};

extern Fvector2 g_current_font_scale;
extern float g_fontHeightScale;
extern float g_fontWidthScale;
