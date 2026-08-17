#include "stdafx.h"

#include "dxStatGraphRender.h"

void dxStatGraphRender::Copy(IStatGraphRender& _in)
{
    auto& in{*smart_cast<const dxStatGraphRender*>(&_in)};

    hGeomTri = in.hGeomTri;
    hGeomLine = in.hGeomLine;
}

void dxStatGraphRender::OnDeviceCreate()
{
    hGeomLine.create(FVF::F_TL0uv, SGeometry::default_vb(), SGeometry::default_ib());
    XR_ASSERT(hGeomLine.stride() == sizeof(FVF::TL0uv));

    hGeomTri.create(FVF::F_TL0uv, SGeometry::default_vb(), RImplementation.QuadIB);
    XR_ASSERT(hGeomTri.stride() == sizeof(FVF::TL0uv));
}

void dxStatGraphRender::OnDeviceDestroy()
{
    hGeomLine.destroy();
    hGeomTri.destroy();
}

void dxStatGraphRender::OnRender(CStatGraph& owner)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    cmd_list.OnFrameEnd();
    RenderBack(cmd_list, owner);

    std::size_t TriElem{0};
    std::size_t LineElem{0};

    for (const auto& sub : owner.subgraphs)
    {
        switch (sub.style)
        {
        case CStatGraph::stBar: TriElem += sub.elements.size() * 4; break;
        case CStatGraph::stCurve: LineElem += sub.elements.size() * 2; break;
        case CStatGraph::stBarLine: LineElem += sub.elements.size() * 4; break;
        default: break;
        }
    }

    if (TriElem > 0)
    {
        const auto verts = cmd_list.Vertex.Lock<FVF::TL0uv>(TriElem);
        std::size_t written{0};

        for (const auto& sub : owner.subgraphs)
        {
            switch (sub.style)
            {
            case CStatGraph::stBar: written += RenderBars(verts.subspan(written, sub.elements.size() * 4), owner, sub.elements); break;
            default: break;
            }
        }

        const auto dwOffsetTri = cmd_list.Vertex.Unlock<FVF::TL0uv>(written);

        cmd_list.set_Geometry(hGeomTri);
        cmd_list.Render(D3DPT_TRIANGLELIST, dwOffsetTri, 0, written, 0, written / 2);
    }

    if (LineElem > 0)
    {
        const auto verts = cmd_list.Vertex.Lock<FVF::TL0uv>(LineElem);
        std::size_t written{0};

        for (const auto& sub : owner.subgraphs)
        {
            switch (sub.style)
            {
            case CStatGraph::stCurve: written += RenderLines(verts.subspan(written, sub.elements.size() * 2), owner, sub.elements); break;
            case CStatGraph::stBarLine: written += RenderBarLines(verts.subspan(written, sub.elements.size() * 4), owner, sub.elements); break;
            default: break;
            }
        }

        const auto dwOffsetLine = cmd_list.Vertex.Unlock<FVF::TL0uv>(written);

        cmd_list.set_Geometry(hGeomLine);
        cmd_list.Render(D3DPT_LINELIST, dwOffsetLine, written / 2);
    }

    if (owner.m_Markers.empty())
        return;

    LineElem = owner.m_Markers.size() * 2;
    const auto verts = cmd_list.Vertex.Lock<FVF::TL0uv>(LineElem);

    RenderMarkers(verts, owner, owner.m_Markers);
    const auto dwOffsetLine = cmd_list.Vertex.Unlock<FVF::TL0uv>(LineElem);

    cmd_list.set_Geometry(hGeomLine);
    cmd_list.Render(D3DPT_LINELIST, dwOffsetLine, LineElem / 2);
}

void dxStatGraphRender::RenderBack(CBackend& cmd_list, const CStatGraph& owner)
{
    // draw back
    auto verts = cmd_list.Vertex.Lock<FVF::TL0uv>(4);

    // base rect
    verts[0].set(owner.lt.x, owner.rb.y, owner.back_color);
    verts[1].set(owner.lt.x, owner.lt.y, owner.back_color);
    verts[2].set(owner.rb.x, owner.rb.y, owner.back_color);
    verts[3].set(owner.rb.x, owner.lt.y, owner.back_color);

    // render
    auto dwOffset = cmd_list.Vertex.Unlock<FVF::TL0uv>(4);

    cmd_list.set_Geometry(hGeomTri);
    cmd_list.Render(D3DPT_TRIANGLELIST, dwOffset, 0, 4, 0, 2);

    // draw rect
    verts = cmd_list.Vertex.Lock<FVF::TL0uv>(5);

    // base rect
    verts[0].set(owner.lt.x, owner.lt.y, owner.rect_color);
    verts[1].set(owner.rb.x - 1, owner.lt.y, owner.rect_color);
    verts[2].set(owner.rb.x - 1, owner.rb.y, owner.rect_color);
    verts[3].set(owner.lt.x, owner.rb.y, owner.rect_color);
    verts[4].set(owner.lt.x, owner.lt.y, owner.rect_color);

    // render
    dwOffset = cmd_list.Vertex.Unlock<FVF::TL0uv>(5);

    cmd_list.set_Geometry(hGeomLine);
    cmd_list.Render(D3DPT_LINESTRIP, dwOffset, 4);

    // draw owner.grid
    float elem_factor = float(owner.rb.y - owner.lt.y) / float(owner.mx - owner.mn);
    float base_y = float(owner.rb.y) + (owner.mn * elem_factor);

    int PNum_H_LinesUp = int((base_y - float(owner.lt.y)) / (owner.grid_step.y * elem_factor));
    int PNum_H_LinesDwn = u32((float(owner.rb.y) - base_y) / (owner.grid_step.y * elem_factor));
    int Num_H_LinesUp = (owner.grid.y < PNum_H_LinesUp) ? owner.grid.y : PNum_H_LinesUp;
    int Num_H_LinesDwn = (owner.grid.y < PNum_H_LinesUp) ? owner.grid.y : PNum_H_LinesDwn;

    verts = cmd_list.Vertex.Lock<FVF::TL0uv>(2 + 2 * owner.grid.x + Num_H_LinesUp * 2 + Num_H_LinesDwn * 2);

    // base Coordinate Line
    verts[0].set(owner.lt.x, int(base_y), owner.base_color);
    verts[1].set(owner.rb.x, int(base_y), owner.base_color);

    std::size_t written{2};

    for (auto [v, g_x] : std::views::zip(verts.subspan(written, owner.grid.x * 2) | std::views::chunk(2), std::views::iota(1, owner.grid.x + 1)))
    {
        v[0].set(int(owner.lt.x + g_x * owner.grid_step.x * elem_factor), owner.lt.y, owner.grid_color);
        v[1].set(int(owner.lt.x + g_x * owner.grid_step.x * elem_factor), owner.rb.y, owner.grid_color);
        written += 2;
    }

    for (auto [v, g_y] : std::views::zip(verts.subspan(written, Num_H_LinesDwn * 2) | std::views::chunk(2), std::views::iota(1, Num_H_LinesDwn + 1)))
    {
        v[0].set(owner.lt.x, int(base_y + g_y * owner.grid_step.y * elem_factor), owner.grid_color);
        v[1].set(owner.rb.x, int(base_y + g_y * owner.grid_step.y * elem_factor), owner.grid_color);
        written += 2;
    }

    for (auto [v, g_y] : std::views::zip(verts.subspan(written, Num_H_LinesUp * 2) | std::views::chunk(2), std::views::iota(1, Num_H_LinesUp + 1)))
    {
        v[0].set(owner.lt.x, int(base_y - g_y * owner.grid_step.y * elem_factor), owner.grid_color);
        v[1].set(owner.rb.x, int(base_y - g_y * owner.grid_step.y * elem_factor), owner.grid_color);
        written += 2;
    }

    dwOffset = cmd_list.Vertex.Unlock<FVF::TL0uv>(written);

    cmd_list.set_Geometry(hGeomLine);
    cmd_list.Render(D3DPT_LINELIST, dwOffset, written / 2);
}

std::size_t dxStatGraphRender::RenderBars(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::ElementsDeq& pelements) const
{
    float elem_offs = float(owner.rb.x - owner.lt.x) / owner.max_item_count;
    float elem_factor = float(owner.rb.y - owner.lt.y) / float(owner.mx - owner.mn);
    float base_y = float(owner.rb.y) + (owner.mn * elem_factor);

    float column_width = elem_offs;
    if (column_width > 1)
        column_width--;

    std::size_t written{0};

    for (auto [i, v, elem] : std::views::zip(std::views::indices(std::ssize(pelements)), verts | std::views::chunk(4), pelements))
    {
        const auto X = gsl::narrow_cast<f32>(i) * elem_offs + owner.lt.x;
        const auto Y0 = base_y;
        const auto Y1 = base_y - elem.data * elem_factor;

        if (Y1 > Y0)
        {
            v[0].set(X, Y1, elem.color);
            v[1].set(X, Y0, elem.color);
            v[2].set(X + column_width, Y1, elem.color);
            v[3].set(X + column_width, Y0, elem.color);
        }
        else
        {
            v[0].set(X, Y0, elem.color);
            v[1].set(X, Y1, elem.color);
            v[2].set(X + column_width, Y0, elem.color);
            v[3].set(X + column_width, Y1, elem.color);
        }

        written += 4;
    }

    return written;
}

std::size_t dxStatGraphRender::RenderLines(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::ElementsDeq& pelements) const
{
    float elem_offs = float(owner.rb.x - owner.lt.x) / owner.max_item_count;
    float elem_factor = float(owner.rb.y - owner.lt.y) / float(owner.mx - owner.mn);
    float base_y = float(owner.rb.y) + (owner.mn * elem_factor);

    std::size_t written{0};

    for (auto [i, v, elem] :
         std::views::zip(std::views::indices(std::ssize(pelements)) | std::views::pairwise, verts | std::views::chunk(2), pelements | std::views::pairwise))
    {
        v[0].set(gsl::narrow_cast<f32>(std::get<0>(i)) * elem_offs + owner.lt.x, base_y - std::get<0>(elem).data * elem_factor, std::get<1>(elem).color);
        v[1].set(gsl::narrow_cast<f32>(std::get<1>(i)) * elem_offs + owner.lt.x, base_y - std::get<1>(elem).data * elem_factor, std::get<1>(elem).color);
        written += 2;
    }

    return written;
}

std::size_t dxStatGraphRender::RenderBarLines(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::ElementsDeq& pelements) const
{
    float elem_offs = float(owner.rb.x - owner.lt.x) / owner.max_item_count;
    float elem_factor = float(owner.rb.y - owner.lt.y) / float(owner.mx - owner.mn);
    float base_y = float(owner.rb.y) + (owner.mn * elem_factor);

    std::size_t written{0};

    for (auto [i, v, elem] :
         std::views::zip(std::views::indices(std::ssize(pelements)) | std::views::pairwise, verts | std::views::chunk(4), pelements | std::views::pairwise))
    {
        const auto color = std::get<1>(elem).color;

        v[0].set(gsl::narrow_cast<f32>(std::get<0>(i)) * elem_offs + owner.lt.x + elem_offs, base_y - std::get<0>(elem).data * elem_factor, color);

        const auto X1 = gsl::narrow_cast<f32>(std::get<1>(i)) * elem_offs + owner.lt.x;
        const auto Y1 = base_y - std::get<1>(elem).data * elem_factor;

        v[1].set(X1, Y1, color);
        v[2].set(X1, Y1, color);
        v[3].set(X1 + elem_offs, Y1, color);

        written += 4;
    }

    return written;
}

void dxStatGraphRender::RenderMarkers(std::span<FVF::TL0uv> verts, const CStatGraph& owner, const CStatGraph::MarkersDeq& pmarkers) const
{
    float elem_offs = float(owner.rb.x - owner.lt.x) / owner.max_item_count;
    float elem_factor = float(owner.rb.y - owner.lt.y) / float(owner.mx - owner.mn);
    float base_y = float(owner.rb.y) + (owner.mn * elem_factor);

    for (auto [v, CurMarker] : std::views::zip(verts | std::views::chunk(2), pmarkers))
    {
        float X0 = 0, Y0 = 0, X1 = 0, Y1 = 0;

        switch (CurMarker.m_eStyle)
        {
        case CStatGraph::stVert:
            X0 = CurMarker.m_fPos * elem_offs + owner.lt.x;
            clamp(X0, float(owner.lt.x), float(owner.rb.x));
            X1 = X0;
            Y0 = float(owner.lt.y);
            Y1 = float(owner.rb.y);
            break;
        case CStatGraph::stHor:
            X0 = float(owner.lt.x);
            X1 = float(owner.rb.x);
            Y0 = base_y - CurMarker.m_fPos * elem_factor;
            clamp(Y0, float(owner.lt.y), float(owner.rb.y));
            Y1 = Y0;
            break;
        default: break;
        }

        v[0].set(X0, Y0, CurMarker.m_dwColor);
        v[1].set(X1, Y1, CurMarker.m_dwColor);
    }
}
