#include "stdafx.h"

#include "flod.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

void R_dsgraph_structure::render_lods()
{
    if (mapLOD.empty())
        return;

    // *** Fill VB and generate groups
    constexpr u32 shid{4};
    auto iter = mapLOD.begin();
    FLOD* firstV = (FLOD*)iter->second.pVisual;
    ref_selement cur_S = firstV->shader->E[shid];
    float ssaRange = r_ssaLOD_A - r_ssaLOD_B;
    if (ssaRange < EPS_S)
        ssaRange = EPS_S;

    constexpr u32 uiVertexPerImposter = 4;
    const u32 uiImpostersFit = RImplementation.Vertex.GetSize() / (firstV->geom->vb_stride * uiVertexPerImposter);

    for (size_t i = 0; i < mapLOD.size(); i++, iter++)
    {
        const u32 iBatchSize = std::min(mapLOD.size() - i, (size_t)uiImpostersFit);
        u32 cur_count = 0;
        u32 vOffset;
        FLOD::_hw* V = (FLOD::_hw*)RImplementation.Vertex.Lock(iBatchSize * uiVertexPerImposter, firstV->geom->vb_stride, vOffset);

        for (u32 j = 0; j < iBatchSize; ++j, ++i, iter++)
        {
            // sort out redundancy
            R_dsgraph::_LodItem& P = iter->second;
            if (P.pVisual->shader->E[shid] == cur_S)
                cur_count++;
            else
            {
                lstLODgroups.push_back(cur_count);
                cur_S = P.pVisual->shader->E[shid];
                cur_count = 1;
            }

            // calculate alpha
            float ssaDiff = P.ssa - r_ssaLOD_B;
            float scale = ssaDiff / ssaRange;
            int iA = iFloor((1 - scale) * 255.f);
            u32 uA = u32(clampr(iA, 0, 255));

            // calculate direction and shift
            FLOD* lodV = (FLOD*)P.pVisual;
            Fvector Ldir, shift;
            Ldir.sub(lodV->vis.sphere.P, Device.vCameraPosition).normalize();
            shift.mul(Ldir, -.5f * lodV->vis.sphere.R);

            // gen geometry
            FLOD::_face* facets = lodV->facets;
            svector<std::pair<float, u32>, 8> selector;
            for (u32 s = 0; s < 8; s++)
                selector.push_back(std::make_pair(Ldir.dotproduct(facets[s].N), s));
            std::ranges::sort(selector, [](const auto& v1, const auto& v2) { return v1.first < v2.first; });

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
            static constexpr int vid[4] = {3, 0, 2, 1};
            for (int id : vid)
            {
                V->p0.add(FB.v[id].v, shift);
                V->p1.add(FA.v[id].v, shift);
                V->n0 = FB.N;
                V->n1 = FA.N;
                V->sun_af = color_rgba(FB.v[id].c_sun, FA.v[id].c_sun, uA, uF);
                V->t0x = FB.v[id].tx;
                V->t0y = FB.v[id].ty;
                V->t1x = FA.v[id].tx;
                V->t1y = FA.v[id].ty;
                V->rgbh0 = FB.v[id].c_rgb_hemi;
                V->rgbh1 = FA.v[id].c_rgb_hemi;
                V++;
            }
        }
        lstLODgroups.push_back(cur_count);
        RImplementation.Vertex.Unlock(iBatchSize * uiVertexPerImposter, firstV->geom->vb_stride);

        // *** Render
        cmd_list.set_xform_world(Fidentity);
        for (u32 uiPass = 0; uiPass < SHADER_PASSES_MAX; ++uiPass)
        {
            auto current = mapLOD.begin();
            u32 vCurOffset = vOffset;

            for (u32 p_count : lstLODgroups)
            {
                u32 uiNumPasses = current->second.pVisual->shader->E[shid]->passes.size();
                if (uiPass < uiNumPasses)
                {
                    cmd_list.set_Element(current->second.pVisual->shader->E[shid], uiPass);
                    cmd_list.set_Geometry(firstV->geom);
                    cmd_list.Render(D3DPT_TRIANGLELIST, vCurOffset, 0, 4 * p_count, 0, 2 * p_count);
                }
                cmd_list.stat.r.s_flora_lods.add(4 * p_count);
                current += p_count;
                vCurOffset += 4 * p_count;
            }
        }

        lstLODgroups.clear();
    }

    mapLOD.clear();
}
