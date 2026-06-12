#ifndef IRENDERABLE_H_INCLUDED
#define IRENDERABLE_H_INCLUDED

#include "render.h"

//////////////////////////////////////////////////////////////////////////
// definition ("Renderable")

class XR_NOVTABLE IRenderable : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRenderable);

public:
    struct
    {
        Fmatrix xform;
        IRenderVisual* visual{};
        IRender_ObjectSpecific* pROS{};
        bool pROS_Allowed{true};
        bool hud{};
    } renderable;

public:
    IRenderable();
    ~IRenderable() override;

    [[nodiscard]] IRender_ObjectSpecific* renderable_ROS();
    [[nodiscard]] virtual bool renderable_HUD() const { return renderable.hud; }
    virtual void renderable_HUD(bool value) { renderable.hud = value; }
    virtual void renderable_Render(u32 context_id, IRenderable* root) = 0;
    [[nodiscard]] virtual BOOL renderable_ShadowGenerate() { return false; }
    [[nodiscard]] virtual BOOL renderable_ShadowReceive() { return false; }
};

#endif // IRENDERABLE_H_INCLUDED
