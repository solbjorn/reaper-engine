#pragma once

#include "iinputreceiver.h"

extern Flags32 psHUD_Flags;

enum HUD_Flags : u32
{
    HUD_CROSSHAIR = 1 << 0,
    HUD_CROSSHAIR_DIST = 1 << 1,
    // = 1 << 2,
    HUD_INFO = 1 << 3,
    HUD_DRAW = 1 << 4,
    HUD_CROSSHAIR_RT = 1 << 5,
    HUD_WEAPON_RT = 1 << 6,
    HUD_CROSSHAIR_DYNAMIC = 1 << 7,
    HUD_CROSSHAIR_HARD = 1 << 8, // Усложненный режим прицела - оружие от бедра будет стрелять не по центру камеры, а по реальному направлению ствола
    HUD_CROSSHAIR_RT2 = 1 << 9,
    HUD_DRAW_RT = 1 << 10,
    HUD_CROSSHAIR_BUILD = 1 << 11, // старый стиль курсора
};

class CUI;

class XR_NOVTABLE CCustomHUD : public DLL_Pure, public IEventReceiver
{
    RTTI_DECLARE_TYPEINFO(CCustomHUD, DLL_Pure, IEventReceiver);

public:
    CCustomHUD();
    ~CCustomHUD() override;

    virtual void Load() {}

    virtual void Render_First(ctx_id_t) {}
    virtual void Render_Last(ctx_id_t) {}

    virtual void OnFrame() {}
    tmc::task<void> OnEvent(EVENT, u64, u64) override { co_return; }

    virtual IC CUI* GetUI() = 0;
    virtual void OnScreenRatioChanged() = 0;
    virtual void OnDisconnected() = 0;
    virtual void OnConnected() = 0;
    virtual void RenderActiveItemUI() = 0;
    virtual bool RenderActiveItemUIQuery() = 0;
    virtual void net_Relcase(CObject* object) = 0;
};

extern CCustomHUD* g_hud;
