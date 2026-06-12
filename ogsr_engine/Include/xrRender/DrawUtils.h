//----------------------------------------------------
// file: DrawUtils.h
//----------------------------------------------------

#ifndef DrawUtilsH
#define DrawUtilsH

//----------------------------------------------------
// Utilities
//----------------------------------------------------
class XR_NOVTABLE CDUInterface : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CDUInterface);

public:
    ~CDUInterface() override = 0;

    //----------------------------------------------------
    virtual void DrawCross(const Fvector3& p, f32 szx1, f32 szy1, f32 szz1, f32 szx2, f32 szy2, f32 szz2, u32 clr, BOOL bRot45 = false) = 0;
    virtual void DrawCross(const Fvector3& p, f32 sz, u32 clr, BOOL bRot45 = false) = 0;
    virtual void DrawFlag(const Fvector3& p, f32 heading, f32 height, f32 sz, f32 sz_fl, u32 clr, BOOL bDrawEntity) = 0;
    virtual void DrawRomboid(const Fvector3& p, f32 radius, u32 clr) = 0;
    virtual void DrawJoint(const Fvector3& p, f32 radius, u32 clr) = 0;

    virtual void DrawSpotLight(const Fvector3& p, const Fvector3& d, f32 range, f32 phi, u32 clr) = 0;
    virtual void DrawDirectionalLight(const Fvector3& p, const Fvector3& d, f32 radius, f32 range, u32 clr) = 0;
    virtual void DrawPointLight(const Fvector3& p, f32 radius, u32 clr) = 0;

    virtual void DrawSound(const Fvector3& p, f32 radius, u32 clr) = 0;
    virtual void DrawLineSphere(const Fvector3& p, f32 radius, u32 clr, BOOL bCross) = 0;

    virtual void dbgDrawPlacement(const Fvector3& p, s32 sz, u32 clr, gsl::czstring caption = nullptr, u32 clr_font = std::numeric_limits<u32>::max()) = 0;
    virtual void dbgDrawVert(const Fvector3& p0, u32 clr, gsl::czstring caption = nullptr) = 0;
    virtual void dbgDrawEdge(const Fvector3& p0, const Fvector3& p1, u32 clr, gsl::czstring caption = nullptr) = 0;
    virtual void dbgDrawFace(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, u32 clr, gsl::czstring caption = nullptr) = 0;

    virtual void DrawFace(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
    virtual void DrawLine(const Fvector3& p0, const Fvector3& p1, u32 clr) = 0;
    virtual void DrawLink(const Fvector3& p0, const Fvector3& p1, f32 sz, u32 clr) = 0;
    virtual void DrawFaceNormal(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, f32 size, u32 clr) = 0;
    virtual void DrawFaceNormal(const Fvector3* p, f32 size, u32 clr) = 0;
    virtual void DrawFaceNormal(const Fvector3& C, const Fvector3& N, f32 size, u32 clr) = 0;
    virtual void DrawSelectionBox(const Fvector3& center, const Fvector3& size, u32* c = nullptr) = 0;
    virtual void DrawSelectionBoxB(const Fbox& box, u32* c = nullptr) = 0;
    virtual void DrawIdentSphere(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
    virtual void DrawIdentSpherePart(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
    virtual void DrawIdentCone(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
    virtual void DrawIdentCylinder(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
    virtual void DrawIdentBox(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;

    virtual void DrawBox(const Fvector3& offs, const Fvector3& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
    virtual void DrawAABB(const Fvector3& p0, const Fvector3& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
    virtual void DrawAABB(const Fmatrix& parent, const Fvector3& center, const Fvector3& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
    virtual void DrawOBB(const Fmatrix& parent, const Fobb& box, u32 clr_s, u32 clr_w) = 0;
    virtual void DrawSphere(const Fmatrix& parent, const Fvector3& center, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
    virtual void DrawSphere(const Fmatrix& parent, const Fsphere& S, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
    virtual void DrawCylinder(const Fmatrix& parent, const Fvector3& center, const Fvector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid,
                              BOOL bWire) = 0;
    virtual void DrawCone(const Fmatrix& parent, const Fvector3& apex, const Fvector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid,
                          BOOL bWire) = 0;
    virtual void DrawPlane(const Fvector3& center, const Fvector2& scale, const Fvector3& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid,
                           BOOL bWire) = 0;
    virtual void DrawPlane(const Fvector3& p, const Fvector3& n, const Fvector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire) = 0;
    virtual void DrawRectangle(const Fvector3& o, const Fvector3& u, const Fvector3& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;

    virtual void DrawGrid() = 0;
    virtual void DrawPivot(const Fvector3& pos, f32 sz = 5.0f) = 0;
    virtual void DrawAxis(const Fmatrix& T) = 0;
    virtual void DrawObjectAxis(const Fmatrix& T, f32 sz, BOOL sel) = 0;
    virtual void DrawSelectionRect(const Ivector2& m_SelStart, const Ivector2& m_SelEnd) = 0;

    virtual void DrawIndexedPrimitive(s32 prim_type, u32 pc, const Fvector3& pos, const Fvector3* vb, const u32& vb_size, const u32* ib, const u32& ib_size,
                                      const u32& clr_argb, f32 scale = 1.0f) = 0;

    virtual void OutText(const Fvector3& pos, gsl::czstring text, u32 color = 0xFF000000, u32 shadow_color = 0xFF909090) = 0;

    virtual void OnDeviceDestroy() = 0;
};

inline CDUInterface::~CDUInterface() = default;
//----------------------------------------------------

#endif
