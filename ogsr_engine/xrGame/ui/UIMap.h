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
    virtual ~CUICustomMap();

    virtual void SetActivePoint(const Fvector& vNewPoint);

    virtual void Init(shared_str name, CInifile& gameLtx, LPCSTR sh_name);

    virtual Fvector2 ConvertRealToLocal(const Fvector2& src, bool for_drawing); // meters->pixels (relatively own left-top pos)
    Fvector2 ConvertLocalToReal(const Fvector2& src) const;
    Fvector2 ConvertRealToLocalNoTransform(const Fvector2& src) const; // meters->pixels (relatively own left-top pos)
    Fvector2 ConvertRealToLocalNoTransform(const Fvector2& src, Frect const& bound_rect) const;

    virtual bool GetPointerTo(const Fvector2& src, float item_radius, Fvector2& pos, float& heading); // position and heading for drawing pointer to src pos

    void FitToWidth(float width);
    void FitToHeight(float height);

    float GetCurrentZoom() const { return GetWndRect().width() / m_BoundRect.width(); }
    const Frect& BoundRect() const { return m_BoundRect; };
    virtual void OptimalFit(const Frect& r);

    shared_str MapName() { return m_name; }

    virtual void Update();
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

    virtual bool IsRectVisible(Frect r);
    virtual bool NeedShowPointer(Frect r);

    bool Locked() const { return !!m_flags.test(eLocked); }
    void SetLocked(bool b) { m_flags.set(eLocked, b); }

    bool IsRounded() const { return m_flags.test(eRounded); }
    void SetRounded(bool b) { m_flags.set(eRounded, b); }

    void SetPointerDistance(float d) { m_pointer_dist = d; };
    float GetPointerDistance() const { return m_pointer_dist; };

protected:
    virtual void UpdateSpots() {};
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
    CUIGlobalMap(CUIMapWnd* pMapWnd);
    virtual ~CUIGlobalMap();

    virtual Fvector2 ConvertRealToLocal(const Fvector2& src, bool for_drawing); // pixels->pixels (relatively own left-top pos)

    IC void SetMinZoom(float zoom) { m_min_zoom = zoom; }
    IC float GetMinZoom() const { return m_min_zoom; }
    IC void SetMaxZoom(float zoom) { m_max_zoom = zoom; }
    IC float GetMaxZoom() const { return m_max_zoom; }

    virtual void Init(shared_str name, CInifile& gameLtx, LPCSTR sh_name);
    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);

    CUIMapWnd* MapWnd() const { return m_mapWnd; }

    void MoveWndDelta(const Fvector2& d);

    float CalcOpenRect(const Fvector2& center_point, Frect& map_desired_rect, float tgt_zoom) const;

    void ClipByVisRect();

    virtual void Update();
    virtual void DrawText();
};

class CUILevelMap : public CUICustomMap
{
    RTTI_DECLARE_TYPEINFO(CUILevelMap, CUICustomMap);

public:
    typedef CUICustomMap inherited;

    CUIMapWnd* m_mapWnd{};
    Frect m_GlobalRect{}; // virtual map size (meters)

    CUILevelMap(const CUILevelMap&) = delete;
    CUILevelMap& operator=(const CUILevelMap&) = delete;

    CUILevelMap(CUIMapWnd*);
    virtual ~CUILevelMap();

    virtual void Init(shared_str name, CInifile& gameLtx, LPCSTR sh_name);

    const Frect& GlobalRect() const { return m_GlobalRect; }

    virtual void Draw();
    virtual void Update();
    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

    // Frect CalcWndRectOnGlobal();
    CUIMapWnd* MapWnd() const { return m_mapWnd; }

    virtual void OnFocusLost();

protected:
    virtual void UpdateSpots();
};

class CUIMiniMap : public CUICustomMap
{
    RTTI_DECLARE_TYPEINFO(CUIMiniMap, CUICustomMap);

public:
    typedef CUICustomMap inherited;

    CUIMiniMap();
    virtual ~CUIMiniMap();

    virtual void Init(shared_str name, CInifile& gameLtx, LPCSTR sh_name);

    virtual void Draw();
    virtual bool GetPointerTo(const Fvector2& src, float item_radius, Fvector2& pos, float& heading); // position and heading for drawing pointer to src pos
    virtual bool NeedShowPointer(Frect r);
    virtual bool IsRectVisible(Frect r);

protected:
    virtual void UpdateSpots();
};
