//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------

#ifndef D3DUtilsH
#define D3DUtilsH

#include "..\..\Include\xrRender\DrawUtils.h"
//----------------------------------------------------

#define DU_DRAW_DIP RCache.dbg_DIP
#define DU_DRAW_DP RCache.dbg_DP

struct SPrimitiveBuffer
{
    ref_geom pGeom{};
    u32 v_cnt;
    u32 i_cnt;
    D3DPRIMITIVETYPE p_type;
    u32 p_cnt;
    CallMe::Delegate<void()> OnRender;
    void RenderDIP() { DU_DRAW_DIP(p_type, pGeom, 0, 0, v_cnt, 0, p_cnt); }
    void RenderDP() { DU_DRAW_DP(p_type, pGeom, 0, p_cnt); }

public:
    SPrimitiveBuffer() : OnRender(CallMe::Delegate<void()>()) {}

    void CreateFromData(D3DPRIMITIVETYPE, u32, u32, const void*, u32, const u16* = nullptr, u32 = 0) {}
    void Destroy();
    void Render() { OnRender(); }
};

//----------------------------------------------------
// Utilities
//----------------------------------------------------

class CDrawUtilities : public CDUInterface, public pureRender
{
    RTTI_DECLARE_TYPEINFO(CDrawUtilities, CDUInterface, pureRender);

public:
    SPrimitiveBuffer m_SolidCone;
    SPrimitiveBuffer m_WireCone;
    SPrimitiveBuffer m_SolidSphere;
    SPrimitiveBuffer m_WireSphere;
    SPrimitiveBuffer m_SolidSpherePart;
    SPrimitiveBuffer m_WireSpherePart;
    SPrimitiveBuffer m_SolidCylinder;
    SPrimitiveBuffer m_WireCylinder;
    SPrimitiveBuffer m_SolidBox;
    SPrimitiveBuffer m_WireBox;
    CGameFont* m_Font{};

    ref_geom vs_L{};
    ref_geom vs_TL{};
    ref_geom vs_LIT{};

protected:
    FVF::L* m_DD_pv{};
    FVF::L* m_DD_pv_start{};
    u32 m_DD_base{};
    BOOL m_DD_wire{};
    void DD_DrawFace_flush(BOOL try_again);

public:
    void DD_DrawFace_begin(BOOL bWire);
    void DD_DrawFace_push(const Fvector& p0, const Fvector& p1, const Fvector& p2, u32 clr);
    void DD_DrawFace_end();

public:
    CDrawUtilities() = default;
    ~CDrawUtilities() override = default;

    void OnDeviceCreate();
    void OnDeviceDestroy() override;

    void UpdateGrid(int number_of_cell, float square_size, int subdiv = 10);

    //----------------------------------------------------
    void DrawCross(const Fvector3& p, f32 szx1, f32 szy1, f32 szz1, f32 szx2, f32 szy2, f32 szz2, u32 clr, BOOL bRot45 = false) override;
    void DrawCross(const Fvector3& p, f32 sz, u32 clr, BOOL bRot45 = false) override { DrawCross(p, sz, sz, sz, sz, sz, sz, clr, bRot45); }
    virtual void DrawEntity(u32 clr, ref_shader s);
    void DrawFlag(const Fvector3& p, f32 heading, f32 height, f32 sz, f32 sz_fl, u32 clr, BOOL bDrawEntity) override;
    void DrawRomboid(const Fvector3& p, f32 radius, u32 clr) override;
    void DrawJoint(const Fvector3& p, f32 radius, u32 clr) override;

    void DrawSpotLight(const Fvector3& p, const Fvector3& d, f32 range, f32 phi, u32 clr) override;
    void DrawDirectionalLight(const Fvector3& p, const Fvector3& d, f32 radius, f32 range, u32 clr) override;
    void DrawPointLight(const Fvector3& p, f32 radius, u32 clr) override;

    void DrawSound(const Fvector3& p, f32 radius, u32 clr) override;
    void DrawLineSphere(const Fvector3& p, f32 radius, u32 clr, BOOL bCross) override;

    void dbgDrawPlacement(const Fvector3& p, s32 sz, u32 clr, gsl::czstring caption = nullptr, u32 clr_font = std::numeric_limits<u32>::max()) override;
    void dbgDrawVert(const Fvector3& p0, u32 clr, gsl::czstring caption = nullptr) override;
    void dbgDrawEdge(const Fvector3& p0, const Fvector3& p1, u32 clr, gsl::czstring caption = nullptr) override;
    void dbgDrawFace(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, u32 clr, gsl::czstring caption = nullptr) override;

    void DrawFace(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) override;
    void DrawLine(const Fvector3& p0, const Fvector3& p1, u32 clr) override;
    void DrawLink(const Fvector3& p0, const Fvector3& p1, f32 sz, u32 clr) override;

    void DrawFaceNormal(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, f32 size, u32 clr) override
    {
        Fvector N, C, P;
        N.mknormal(p0, p1, p2);
        C.set(p0);
        C.add(p1);
        C.add(p2);
        C.div(3);
        P.mad(C, N, size);
        DrawLine(C, P, clr);
    }

    void DrawFaceNormal(const Fvector3* p, f32 size, u32 clr) override { DrawFaceNormal(p[0], p[1], p[2], size, clr); }

    void DrawFaceNormal(const Fvector3& C, const Fvector3& N, f32 size, u32 clr) override
    {
        Fvector P;
        P.mad(C, N, size);
        DrawLine(C, P, clr);
    }

    void DrawSelectionBox(const Fvector3& center, const Fvector3& size, u32* c = nullptr) override;

    void DrawSelectionBoxB(const Fbox& box, u32* c = nullptr) override
    {
        Fvector S, C;
        box.getsize(S);
        box.getcenter(C);
        DrawSelectionBox(C, S, c);
    }

    void DrawIdentSphere(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) override;
    void DrawIdentSpherePart(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) override;
    void DrawIdentCone(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) override;
    void DrawIdentCylinder(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) override;
    void DrawIdentBox(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) override;

    void DrawBox(const Fvector3& offs, const Fvector3& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) override;
    void DrawAABB(const Fvector3& p0, const Fvector3& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) override;
    void DrawAABB(const Fmatrix& parent, const Fvector3& center, const Fvector3& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) override;
    void DrawOBB(const Fmatrix& parent, const Fobb& box, u32 clr_s, u32 clr_w) override;
    void DrawSphere(const Fmatrix& parent, const Fvector3& center, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) override;

    void DrawSphere(const Fmatrix& parent, const Fsphere& S, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) override
    {
        DrawSphere(parent, S.P, S.R, clr_s, clr_w, bSolid, bWire);
    }

    void DrawCylinder(const Fmatrix& parent, const Fvector3& center, const Fvector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid,
                      BOOL bWire) override;
    void DrawCone(const Fmatrix& parent, const Fvector3& apex, const Fvector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid,
                  BOOL bWire) override;
    void DrawPlane(const Fvector3& center, const Fvector2& scale, const Fvector3& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire) override;
    void DrawPlane(const Fvector3& p, const Fvector3& n, const Fvector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire) override;
    void DrawRectangle(const Fvector3& o, const Fvector3& u, const Fvector3& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) override;

    void DrawGrid() override;
    void DrawPivot(const Fvector3& pos, f32 sz = 5.0f) override;
    void DrawAxis(const Fmatrix& T) override;
    void DrawObjectAxis(const Fmatrix& T, f32 sz, BOOL sel) override;
    void DrawSelectionRect(const Ivector2& m_SelStart, const Ivector2& m_SelEnd) override;

    void DrawIndexedPrimitive(s32, u32, const Fvector3&, const Fvector3*, const u32&, const u32*, const u32&, const u32&, f32 = 1.0f) override {}

    virtual void DrawPrimitiveL(D3DPRIMITIVETYPE pt, u32 pc, Fvector* vertices, int vc, u32 color, BOOL bCull, BOOL bCycle);
    virtual void DrawPrimitiveTL(D3DPRIMITIVETYPE pt, u32 pc, FVF::TL* vertices, int vc, BOOL bCull, BOOL bCycle);
    virtual void DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, u32 pc, FVF::LIT* vertices, int vc, BOOL bCull, BOOL bCycle);

    void OutText(const Fvector3& pos, gsl::czstring text, u32 color = 0xFF000000, u32 shadow_color = 0xFF909090) override;

    tmc::task<void> OnRender() override;
};

extern CDrawUtilities DUImpl;
//----------------------------------------------------

#endif /*_INCDEF_D3DUtils_H_*/
