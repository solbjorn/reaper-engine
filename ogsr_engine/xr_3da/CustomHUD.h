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

class CCustomHUD : public DLL_Pure, public IEventReceiver
{
    RTTI_DECLARE_TYPEINFO(CCustomHUD, DLL_Pure, IEventReceiver);

public:
    CCustomHUD();
    virtual ~CCustomHUD();

    virtual void Load() { ; }

    virtual void Render_First(u32 context_id) {}
    virtual void Render_Last(u32 context_id) {}

    virtual void OnFrame() { ; }
    virtual void OnEvent(EVENT E, u64 P1, u64 P2) { ; }

    virtual IC CUI* GetUI() = 0;
    virtual void OnScreenRatioChanged() = 0;
    virtual void OnDisconnected() = 0;
    virtual void OnConnected() = 0;
    virtual void RenderActiveItemUI() = 0;
    virtual bool RenderActiveItemUIQuery() = 0;
    virtual void net_Relcase(CObject* object) = 0;
};

extern CCustomHUD* g_hud;
