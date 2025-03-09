#ifndef IRENDERABLE_H_INCLUDED
#define IRENDERABLE_H_INCLUDED

#include "render.h"

//////////////////////////////////////////////////////////////////////////
// definition ("Renderable")
class IRenderable
{
public:
    struct
    {
        Fmatrix xform;
        IRenderVisual* visual;
        IRender_ObjectSpecific* pROS;
        BOOL pROS_Allowed;
        bool hud{};
    } renderable;

public:
    IRenderable();
    virtual ~IRenderable();
    IRender_ObjectSpecific* renderable_ROS();
    virtual bool renderable_HUD() const { return renderable.hud; }
    virtual void renderable_HUD(bool value) { renderable.hud = value; }
    virtual void renderable_Render(u32 context_id, IRenderable* root) = 0;
    virtual BOOL renderable_ShadowGenerate() { return FALSE; }
    virtual BOOL renderable_ShadowReceive() { return FALSE; }
};

#endif // IRENDERABLE_H_INCLUDED
