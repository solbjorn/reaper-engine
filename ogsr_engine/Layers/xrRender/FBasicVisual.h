#ifndef FBasicVisualH
#define FBasicVisualH

#include "../../Include/xrRender/RenderVisual.h"
#include "../../xr_3da/vis_common.h"

#define VLOAD_NOVERTICES (1 << 0)

// The class itself
class CKinematicsAnimated;
class CKinematics;
class IParticleCustom;

struct IRender_Mesh : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRender_Mesh);

public:
    // format
    ref_geom rm_geom;

    // verts
    ID3DVertexBuffer* p_rm_Vertices{};
    u32 vBase;
    u32 vCount;

    // indices
    ID3DIndexBuffer* p_rm_Indices{};
    u32 iBase;
    u32 iCount;
    u32 dwPrimitives;

    IRender_Mesh() = default;
    virtual ~IRender_Mesh();

private:
    IRender_Mesh(const IRender_Mesh& other);
    void operator=(const IRender_Mesh& other);
};

// The class itself
class dxRender_Visual : public IRenderVisual
{
    RTTI_DECLARE_TYPEINFO(dxRender_Visual, IRenderVisual);

public:
    shared_str dbg_name;
    virtual shared_str getDebugName() { return dbg_name; }

    // Common data for rendering
    ref_shader shader{}; // pipe state, shared
    u32 Type{}; // visual's type
    bool IsHudVisual{};

    /************************* Add by Zander *******************************/
private:
    bool renderFlag{true}; // if false, don`t push this to render / add by Zander

public:
    vis_data vis; // visibility-data

    inline bool getRZFlag() const { return renderFlag; }
    inline void setRZFlag(const bool f) { renderFlag = f; }
    /************************* End add *************************************/

    virtual void Render(CBackend&, float, bool) {} // LOD - Level Of Detail  [0..1], Ignored
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Release(); // Shared memory release
    virtual void Copy(dxRender_Visual* from);
    virtual void Spawn() {}
    virtual void Depart() {}

    virtual vis_data& getVisData() { return vis; }
    u32 getType() const override { return Type; }

    dxRender_Visual();
    virtual ~dxRender_Visual();
};

#endif // !FBasicVisualH
