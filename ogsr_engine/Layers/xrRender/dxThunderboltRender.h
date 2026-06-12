#ifndef dxThunderboltRender_included
#define dxThunderboltRender_included

#include "..\..\Include\xrRender\ThunderboltRender.h"

class dxThunderboltRender : public IThunderboltRender
{
    RTTI_DECLARE_TYPEINFO(dxThunderboltRender, IThunderboltRender);

public:
    dxThunderboltRender();
    ~dxThunderboltRender() override;

    void Copy(IThunderboltRender& _in) override;

    void Render(CEffect_Thunderbolt& owner) override;

private:
    ref_geom hGeom_model;
    ref_geom hGeom_gradient;
};

#endif //	ThunderboltRender_included
