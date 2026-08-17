#include "stdafx.h"

#include "FLOD.h"

#include "../../xr_3da/IGame_Persistent.h"
#include "../../xr_3da/Environment.h"

void R_dsgraph_structure::render_lods()
{
    if (mapLOD.empty())
        return;

    XR_TRACY_ZONE_SCOPED();

    // *** Fill VB and generate groups
    constexpr auto shid{4uz};
    const auto ssaRange = std::max(r_ssaLOD_A - r_ssaLOD_B, EPS_S);

    static constexpr auto uiVertexPerImposter{4uz};
    // 16-bit Quad IB addresses at most 65536 vertices per draw
    static constexpr auto uiImpostersMax = (std::numeric_limits<u16>::max() + 1uz) / uiVertexPerImposter;

    const auto uiImpostersFit = std::min(cmd_list.Vertex.GetSize() / (sizeof(FLOD::_hw) * uiVertexPerImposter), uiImpostersMax);

    for (auto batch = mapLOD.begin(), end = mapLOD.end(); batch != end;)
    {
        const auto iBatchSize = std::min(gsl::narrow_cast<std::size_t>(std::distance(batch, end)), uiImpostersFit);

        auto cur_S = batch->second.pVisual->shader->E[shid];
        std::size_t cur_count{0};
        auto iter = batch;

        const auto verts = cmd_list.Vertex.Lock<FLOD::_hw>(iBatchSize * uiVertexPerImposter);
        std::size_t written{0};

        for (std::size_t j{0}; j < iBatchSize; ++j)
        {
            // sort out redundancy
            auto& P = iter->second;

            if (P.pVisual->shader->E[shid] != cur_S)
            {
                lstLODgroups.emplace_back(cur_count);
                cur_count = 1;

                cur_S = P.pVisual->shader->E[shid];
            }
            else
            {
                ++cur_count;
            }

            // calculate alpha
            float ssaDiff = P.ssa - r_ssaLOD_B;
            float scale = ssaDiff / ssaRange;
            int iA = iFloor((1 - scale) * 255.f);
            u32 uA = u32(clampr(iA, 0, 255));

            // calculate direction and shift
            auto lodV = smart_cast<FLOD*>(P.pVisual);
            Fvector Ldir, shift;
            Ldir.sub(lodV->vis.sphere.P, Device.vCameraPosition).normalize();
            shift.mul(Ldir, -.5f * lodV->vis.sphere.R);

            // gen geometry
            FLOD::_face* facets = lodV->facets;
            std::inplace_vector<std::pair<f32, u32>, 8> selector;

            for (u32 s = 0; s < 8; s++)
                selector.emplace_back(Ldir.dotproduct(facets[s].N), s);

            std::ranges::sort(selector, {}, &std::pair<f32, u32>::first);

            const float dot_best = selector[selector.size() - 1].first;
            const float dot_next = selector[selector.size() - 2].first;
            const float dot_next_2 = selector[selector.size() - 3].first;
            size_t id_best = selector[selector.size() - 1].second;
            size_t id_next = selector[selector.size() - 2].second;

            // Now we have two "best" planes, calculate factor, and approx normal
            const float fA = dot_best, fB = dot_next, fC = dot_next_2;
            const float alpha = 0.5f + 0.5f * (1 - (fB - fC) / (fA - fC));
            const int iF = iFloor(alpha * 255.5f);
            const u32 uF = u32(clampr(iF, 0, 255));

            // Fill VB
            const FLOD::_face& FA = facets[id_best];
            const FLOD::_face& FB = facets[id_next];

            for (auto [v, id] : std::views::zip(verts.subspan(written, 4), std::array<std::size_t, 4>{3, 0, 2, 1}))
            {
                v.p0.add(FB.v[id].v, shift);
                v.p1.add(FA.v[id].v, shift);
                v.n0 = FB.N;
                v.n1 = FA.N;
                v.sun_af = color_rgba(FB.v[id].c_sun, FA.v[id].c_sun, uA, uF);
                v.t0x = FB.v[id].tx;
                v.t0y = FB.v[id].ty;
                v.t1x = FA.v[id].tx;
                v.t1y = FA.v[id].ty;
                v.rgbh0 = FB.v[id].c_rgb_hemi;
                v.rgbh1 = FA.v[id].c_rgb_hemi;
            }

            written += 4;
            ++iter;
        }

        lstLODgroups.emplace_back(cur_count);

        const auto vOffset = cmd_list.Vertex.Unlock<FLOD::_hw>(written);

        // *** Render
        cmd_list.set_xform_world(Fidentity);

        for (std::size_t uiPass{0}; uiPass < SHADER_PASSES_MAX; ++uiPass)
        {
            auto vCurOffset = vOffset;
            auto current = batch;

            for (auto p_count : lstLODgroups)
            {
                if (const auto vis = current->second.pVisual; uiPass < vis->shader->E[shid]->passes.size())
                {
                    cmd_list.set_Element(vis->shader->E[shid], uiPass);
                    cmd_list.set_Geometry(smart_cast<FLOD*>(vis)->geom);

                    cmd_list.Render(D3DPT_TRIANGLELIST, vCurOffset, 0, 4 * p_count, 0, 2 * p_count);

                    cmd_list.stat.r.s_flora_lods.add(4 * p_count);
                }

                vCurOffset += 4 * p_count;
                current += p_count;
            }
        }

        lstLODgroups.clear();
        batch = iter;
    }

    mapLOD.clear();
}
