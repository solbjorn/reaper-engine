#ifndef dxStatGraphRender_included
#define dxStatGraphRender_included

#include "..\..\Include\xrRender\StatGraphRender.h"
#include "../../xr_3da/StatGraph.h"

class dxStatGraphRender : public IStatGraphRender
{
    RTTI_DECLARE_TYPEINFO(dxStatGraphRender, IStatGraphRender);

public:
    ~dxStatGraphRender() override = default;

    virtual void Copy(IStatGraphRender& _in);

    virtual void OnDeviceCreate();
    virtual void OnDeviceDestroy();
    virtual void OnRender(CStatGraph& owner);

private:
    void RenderBack(CStatGraph& owner);
    void RenderBars(CStatGraph& owner, FVF::TL0uv** ppv, CStatGraph::ElementsDeq* pelements);
    void RenderBarLines(CStatGraph& owner, FVF::TL0uv** ppv, CStatGraph::ElementsDeq* pelements);
    void RenderLines(CStatGraph& owner, FVF::TL0uv** ppv, CStatGraph::ElementsDeq* pelements);
    void RenderMarkers(CStatGraph& owner, FVF::TL0uv** ppv, CStatGraph::MarkersDeq* pmarkers);

private:
    ref_geom hGeomTri;
    ref_geom hGeomLine;
};

#endif //	dxStatGraphRender_included
