#include "stdafx.h"
#include "dxFontRender.h"

#include "../../xr_3da/GameFont.h"

dxFontRender::dxFontRender() {}

dxFontRender::~dxFontRender()
{
    pShader.destroy();
    pGeom.destroy();
}

void dxFontRender::Initialize(LPCSTR cShader, LPCSTR cTexture)
{
    pShader.create(cShader, cTexture);
    pGeom.create(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
}

extern BOOL g_bRendering;
extern Fvector2 g_current_font_scale;

void dxFontRender::RenderFragment(CGameFont& owner, u32& i, bool shadow_mode, float dX, float dY, u32 length, u32 last)
{
    // fill vertices

    // lock AGP memory
    u32 vOffset;
    FVF::TL* v = (FVF::TL*)RCache.Vertex.Lock(length * 4, pGeom.stride(), vOffset);
    FVF::TL* start = v;

    for (; i < last; i++)
    {
        CGameFont::String& PS = owner.strings[i];

        wide_char wsStr[MAX_MB_CHARS];

        u32 len = owner.IsMultibyte() ? mbhMulti2Wide(wsStr, nullptr, MAX_MB_CHARS, PS.string) : xr_strlen(PS.string);

        if (len)
        {
            float X = float(iFloor(PS.x)) + dX;
            float Y = float(iFloor(PS.y)) + dY;

            float S = PS.height * g_current_font_scale.y * owner.GetHeightScale(); // g_current_font_scale это еще один скейлинг шрифтов для pp эффектов похоже

            float Y2 = Y + S;
            float fSize = 0;

            if (PS.align)
            {
                fSize = owner.IsMultibyte() ? owner.SizeOf_(wsStr) : owner.SizeOf_(PS.string); // уже с  * owner.GetWidthScale()
            }

            switch (PS.align)
            {
            case CGameFont::alCenter: X -= (iFloor(fSize * 0.5f)) * g_current_font_scale.x; break;
            case CGameFont::alRight: X -= iFloor(fSize) * g_current_font_scale.x; break;
            }

            u32 clr, clr2;
            clr2 = clr = PS.color;

            if (owner.uFlags & CGameFont::fsGradient)
            {
                u32 _R = color_get_R(clr) / 2;
                u32 _G = color_get_G(clr) / 2;
                u32 _B = color_get_B(clr) / 2;
                u32 _A = color_get_A(clr);
                clr2 = color_rgba(_R, _G, _B, _A);
            }

            if (shadow_mode)
            {
                // color_argb(220, 20, 20, 20)

                const u32 min_alpha = _min(color_get_A(clr), (u32)220);

                u32 _R = color_get_R(clr);
                u32 _G = color_get_G(clr);
                u32 _B = color_get_B(clr);

                float Y = 0.299f * _R + 0.587f * _G + 0.114f * _B;

                u32 c = Y > 40 ? 20 : 120;

                clr2 = clr = color_argb(min_alpha, c, c, c);
            }

            X -= 0.5f;
            Y -= 0.5f;
            Y2 -= 0.5f;

            for (u32 j = 0; j < len; j++)
            {
                const Fvector l = owner.IsMultibyte() ? owner.GetCharTC(wsStr[1 + j]) : owner.GetCharTC((u8)PS.string[j]);

                const float scw = l.z * g_current_font_scale.x * owner.GetWidthScale();

                const float fTCWidth = l.z / owner.vTS.x;

                if (!fis_zero(l.z))
                {
                    const float tu = (l.x / owner.vTS.x);
                    const float tv = (l.y / owner.vTS.y);

                    v->set(X, Y2, clr2, tu, tv + owner.fTCHeight);
                    v++;
                    v->set(X, Y, clr, tu, tv);
                    v++;

                    v->set(X + scw, Y2, clr2, tu + fTCWidth, tv + owner.fTCHeight);
                    v++;
                    v->set(X + scw, Y, clr, tu + fTCWidth, tv);
                    v++;
                }

                X += scw * owner.GetInterval().x;

                if (owner.IsMultibyte())
                {
                    if (IsSpaceCharacter(wsStr[1 + j]))
                        X += owner.GetfXStep() * owner.GetInterval().x * owner.GetWidthScale();
                }
            }
        }
    }

    // Unlock and draw
    u32 vCount = (u32)(v - start);
    RCache.Vertex.Unlock(vCount, pGeom.stride());
    if (vCount)
    {
        RCache.set_Geometry(pGeom);
        RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, vCount, 0, vCount / 2);
    }
}

void dxFontRender::OnRender(CGameFont& owner)
{
    VERIFY(g_bRendering);

    if (owner.strings.empty()) // early exit if there is no text to render
        return;

    if (pShader)
        RCache.set_Shader(pShader);

    if (!(owner.uFlags & CGameFont::fsValid))
    {
        CTexture* T = RCache.get_ActiveTexture(0);
        owner.vTS.set((int)T->get_Width(), (int)T->get_Height());
        owner.fTCHeight = owner.fHeight / float(owner.vTS.y);
        owner.uFlags |= CGameFont::fsValid;
    }

    for (u32 i = 0; i < owner.strings.size();)
    {
        // calculate first-fit
        int count = 1;
        u32 length = owner.SmartLength(owner.strings[i].string);
        while (i + count < owner.strings.size())
        {
            u32 L = owner.SmartLength(owner.strings[i + count].string);
            if (L + length < MAX_MB_CHARS)
            {
                count++;
                length += L;
            }
            else
                break;
        }

        const u32 last = i + count;

        u32 di = i;
        if (ps_r2_ls_flags_ext.test(R2FLAGEXT_FONT_SHADOWS))
            RenderFragment(owner, di, true, 2, 2, length, last);
        RenderFragment(owner, i, false, 0, 0, length, last);
    }
}
