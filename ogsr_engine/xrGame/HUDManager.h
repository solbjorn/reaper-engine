#pragma once

#include "..\xr_3da\CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"

class CHUDTarget;

struct CFontManager : public pureDeviceReset
{
    RTTI_DECLARE_TYPEINFO(CFontManager, pureDeviceReset);

public:
    CFontManager();
    ~CFontManager() override;

    xr_vector<CGameFont*> m_all_fonts;

    void Render();

    // hud font
    CGameFont* pFontMedium{};
    CGameFont* pFontSmall{};

    CGameFont* pFontDI{};

    CGameFont* pFontArial14{};
    CGameFont* pFontArial21{};
    CGameFont* pFontGraffiti19Russian{};
    CGameFont* pFontGraffiti22Russian{};
    CGameFont* pFontLetterica16Russian{};
    CGameFont* pFontLetterica18Russian{};
    CGameFont* pFontGraffiti32Russian{};
    CGameFont* pFontGraffiti40Russian{};
    CGameFont* pFontGraffiti50Russian{};
    CGameFont* pFontLetterica25{};

    void InitializeFonts();
    void InitializeFont(CGameFont*& F, LPCSTR section, u32 flags = 0);
    LPCSTR GetFontTexName(LPCSTR section);
    CGameFont* InitializeCustomFont(LPCSTR section, u32 flags = 0);

    tmc::task<void> OnDeviceReset() override;
};

class CHUDManager : public CCustomHUD
{
    RTTI_DECLARE_TYPEINFO(CHUDManager, CCustomHUD);

public:
    friend class CUI;

private:
    CUI* pUI{};
    CHitMarker HitMarker;
    CHUDTarget* m_pHUDTarget;
    bool b_online;

    std::recursive_mutex render_lock;

public:
    CHUDManager();
    ~CHUDManager() override;

    tmc::task<void> OnEvent(CEvent*, u64, u64) override;

    void Load() override;

    void Render_First(ctx_id_t context_id) override;
    void Render_Last(ctx_id_t context_id) override;
    void OnFrame() override;

    virtual void RenderUI();

    [[nodiscard]] CUI* GetUI() override { return pUI; }

    void Hit(int idx, float, const Fvector& dir);
    CFontManager& Font() { return *(UI()->Font()); }

    // текущий предмет на который смотрит HUD
    collide::rq_result& GetCurrentRayQuery();

    // устанвка внешнего вида прицела в зависимости от текущей дисперсии
    void SetCrosshairDisp(float dispf, float disps = 0.f);
    void ShowCrosshair(bool show);

    void SetHitmarkType(LPCSTR tex_name);
    void OnScreenRatioChanged() override;
    void OnDisconnected() override;
    void OnConnected() override;

    void RenderActiveItemUI() override;
    [[nodiscard]] bool RenderActiveItemUIQuery() override;

    void net_Relcase(CObject* object) override;

    [[nodiscard]] CHUDTarget* GetTarget() const { return m_pHUDTarget; }
};
