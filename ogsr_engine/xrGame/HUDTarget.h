#pragma once

#include "HUDCrosshair.h"
#include "..\xrcdb\xr_collide_defs.h"

constexpr inline u32 C_ON_ENEMY{D3DCOLOR_XRGB(0xff, 0, 0)};
constexpr inline u32 C_ON_NEUTRAL{D3DCOLOR_XRGB(0xff, 0xff, 0x80)};
constexpr inline u32 C_ON_FRIEND{D3DCOLOR_XRGB(0, 0xff, 0)};
constexpr inline u32 C_DEFAULT{D3DCOLOR_XRGB(0xff, 0xff, 0xff)};

class CHUDManager;

class CHUDTarget
{
private:
    friend class CHUDManager;

private:
    typedef collide::rq_result rq_result;
    typedef collide::rq_results rq_results;

private:
    ui_shader hShader;
    f32 fuzzyShowInfo{};
    f32 m_real_dist{};
    rq_result RQ;
    rq_results RQR;

private:
    bool m_bShowCrosshair;
    CHUDCrosshair HUDCrosshair;

private:
    void net_Relcase(CObject* O);

public:
    CHUDTarget();

    void CursorOnFrame();
    void Render();
    void Load();

    [[nodiscard]] f32 GetDist() const { return RQ.range; }
    [[nodiscard]] f32 GetRealDist() const { return m_real_dist; }
    [[nodiscard]] CObject* GetObj() const { return RQ.O; }
};
