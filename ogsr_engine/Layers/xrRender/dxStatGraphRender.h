#ifndef dxStatGraphRender_included
#define dxStatGraphRender_included

#include "../../Include/xrRender/StatGraphRender.h"
#include "../../xr_3da/StatGraph.h"

class dxStatGraphRender final : public IStatGraphRender
{
    RTTI_DECLARE_TYPEINFO(dxStatGraphRender, IStatGraphRender);

public:
    ~dxStatGraphRender() override = default;

    void Copy(IStatGraphRender& _in) override;

    void OnDeviceCreate() override;
    void OnDeviceDestroy() override;
    void OnRender(CStatGraph& owner) override;

private:
    void RenderBack(CBackend& cmd_list, const CStatGraph& owner);
    [[nodiscard]] std::size_t RenderBars(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::ElementsDeq& pelements) const;
    [[nodiscard]] std::size_t RenderBarLines(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::ElementsDeq& pelements) const;
    [[nodiscard]] std::size_t RenderLines(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::ElementsDeq& pelements) const;
    void RenderMarkers(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::MarkersDeq& pmarkers) const;

private:
    ref_geom hGeomTri;
    ref_geom hGeomLine;
};

#endif //	dxStatGraphRender_included
