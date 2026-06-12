#pragma once

#include "UIStatic.h"
#include "UIWndCallback.h"

class CUIGlobalMapSpot;
class CUIMapWnd;

class CUICustomMap : public CUIStatic, public CUIWndCallback
{
    RTTI_DECLARE_TYPEINFO(CUICustomMap, CUIStatic, CUIWndCallback);

protected:
    shared_str m_name{};
    Frect m_BoundRect{}; // real map size (meters)

    enum EFlags
    {
        eLocked = (1 << 0),
        eRounded = (1 << 1)
    };
    Flags16 m_flags{};

    float m_pointer_dist{};

    Frect m_workingArea{};

public:
    Frect& WorkingArea() { return m_workingArea; }

    CUICustomMap();
    ~CUICustomMap() override;

    virtual void SetActivePoint(const Fvector& vNewPoint);

    virtual void Init(shared_str name, CInifile& gameLtx, gsl::czstring sh_name);

    virtual Fvector2 ConvertRealToLocal(const Fvector2& src, bool for_drawing); // meters->pixels (relatively own left-top pos)
    Fvector2 ConvertLocalToReal(const Fvector2& src) const;
    Fvector2 ConvertRealToLocalNoTransform(const Fvector2& src) const; // meters->pixels (relatively own left-top pos)
    Fvector2 ConvertRealToLocalNoTransform(const Fvector2& src, Frect const& bound_rect) const;

    [[nodiscard]] virtual bool GetPointerTo(const Fvector2& src, f32 item_radius, Fvector2& pos,
                                            f32& heading); // position and heading for drawing pointer to src pos

    void FitToWidth(float width);
    void FitToHeight(float height);

    float GetCurrentZoom() const { return GetWndRect().width() / m_BoundRect.width(); }
    const Frect& BoundRect() const { return m_BoundRect; }
    virtual void OptimalFit(const Frect& r);

    shared_str MapName() { return m_name; }

    void Update() override;
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

    [[nodiscard]] virtual bool IsRectVisible(Frect r);
    [[nodiscard]] virtual bool NeedShowPointer(Frect r);

    bool Locked() const { return !!m_flags.test(eLocked); }
    void SetLocked(bool b) { m_flags.set(eLocked, b); }

    bool IsRounded() const { return m_flags.test(eRounded); }
    void SetRounded(bool b) { m_flags.set(eRounded, b); }

    void SetPointerDistance(float d) { m_pointer_dist = d; }
    float GetPointerDistance() const { return m_pointer_dist; }

protected:
    virtual void UpdateSpots() {}
};

class CUIGlobalMap : public CUICustomMap
{
    RTTI_DECLARE_TYPEINFO(CUIGlobalMap, CUICustomMap);

public:
    typedef CUICustomMap inherited;

private:
    CUIMapWnd* m_mapWnd{};

    float m_min_zoom{};
    float m_max_zoom{};

public:
    explicit CUIGlobalMap(CUIMapWnd* pMapWnd);
    ~CUIGlobalMap() override;

    Fvector2 ConvertRealToLocal(const Fvector2& src, bool) override; // pixels->pixels (relatively own left-top pos)

    IC void SetMinZoom(float zoom) { m_min_zoom = zoom; }
    IC float GetMinZoom() const { return m_min_zoom; }
    IC void SetMaxZoom(float zoom) { m_max_zoom = zoom; }
    IC float GetMaxZoom() const { return m_max_zoom; }

    void Init(shared_str name, CInifile& gameLtx, gsl::czstring sh_name) override;
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;

    CUIMapWnd* MapWnd() const { return m_mapWnd; }

    void MoveWndDelta(const Fvector2& d);

    float CalcOpenRect(const Fvector2& center_point, Frect& map_desired_rect, float tgt_zoom) const;

    void ClipByVisRect();

    void Update() override;
    void DrawText() override;
};

class CUILevelMap : public CUICustomMap
{
    RTTI_DECLARE_TYPEINFO(CUILevelMap, CUICustomMap);

public:
    typedef CUICustomMap inherited;

    CUIMapWnd* m_mapWnd{};
    Frect m_GlobalRect{}; // virtual map size (meters)

    explicit CUILevelMap(CUIMapWnd*);
    ~CUILevelMap() override;

    void Init(shared_str name, CInifile& gameLtx, gsl::czstring sh_name) override;

    const Frect& GlobalRect() const { return m_GlobalRect; }

    void Draw() override;
    void Update() override;
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

    // Frect CalcWndRectOnGlobal();
    CUIMapWnd* MapWnd() const { return m_mapWnd; }

    void OnFocusLost() override;

protected:
    void UpdateSpots() override;
};

class CUIMiniMap : public CUICustomMap
{
    RTTI_DECLARE_TYPEINFO(CUIMiniMap, CUICustomMap);

public:
    typedef CUICustomMap inherited;

    CUIMiniMap();
    ~CUIMiniMap() override;

    void Init(shared_str name, CInifile& gameLtx, gsl::czstring sh_name) override;

    void Draw() override;
    [[nodiscard]] bool GetPointerTo(const Fvector2& src, f32 item_radius, Fvector2& pos, f32& heading) override;
    [[nodiscard]] bool NeedShowPointer(Frect r) override;
    [[nodiscard]] bool IsRectVisible(Frect r) override;

protected:
    void UpdateSpots() override;
};
XR_SOL_BASE_CLASSES(CUIMiniMap);
