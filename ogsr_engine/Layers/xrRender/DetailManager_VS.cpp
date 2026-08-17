#include "stdafx.h"

#include "DetailManager.h"

#include "../../xr_3da/IGame_Persistent.h"
#include "../../xr_3da/Environment.h"

#include "../xrRenderDX10/dx10BufferUtils.h"

namespace
{
constexpr f32 quant{16384.0f};
constexpr auto c_hdr{10uz};
constexpr auto c_size{4uz};

constexpr D3DVERTEXELEMENT9 dwDecl[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // pos
                                        {0, 12, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, // uv
                                        D3DDECL_END()};

struct vertHW final
{
    Fvector3 xyz;
    s16 u;
    s16 v;
    s16 t;
    s16 mid;
};
static_assert(sizeof(vertHW) == 20);

[[nodiscard]] constexpr auto QC(f32 v)
{
    return gsl::narrow<s16>(
        std::clamp(gsl::narrow_cast<s32>(std::floor(v * quant)), s32{std::numeric_limits<s16>::min()}, s32{std::numeric_limits<s16>::max()}));
}
} // namespace

void CDetailManager::hw_Load() { hw_Load_Geom(); }

void CDetailManager::hw_Load_Geom()
{
    // Analyze batch-size
    hw_BatchSize = (HW.Caps.geometry.dwRegisters - c_hdr) / c_size;
    clamp(hw_BatchSize, 0uz, 64uz);

    Msg("* [DETAILS] VertexConsts({}), Batch({})", +HW.Caps.geometry.dwRegisters, hw_BatchSize);

    // Pre-process objects
    std::size_t dwVerts{0};
    std::size_t dwIndices{0};

    for (const auto dp : objects)
    {
        dwVerts += dp->vertices.size() * hw_BatchSize;
        dwIndices += dp->indices.size() * hw_BatchSize;
    }

    Msg("* [DETAILS] {} v({}), {} p", dwVerts, sizeof(vertHW), dwIndices / 3);
    Msg("* [DETAILS] Batch({}), VB({}K), IB({}K)", hw_BatchSize, (dwVerts * sizeof(vertHW)) / 1024, (dwIndices * 2) / 1024);

    // Fill VB
    {
        xr_vector<vertHW> verts;
        verts.reserve(dwVerts);

        const auto total = c_size * hw_BatchSize;

        for (const auto dp : objects)
        {
            const auto invh = 1.0f / (dp->bv_bb.max.y - dp->bv_bb.min.y);

            for (std::size_t mid{0}; mid < total; mid += c_size)
            {
                const auto val = gsl::narrow<s16>(mid);

                for (const auto& vert : dp->vertices)
                {
                    auto& v = verts.emplace_back();

                    v.xyz = vert.P;
                    v.u = QC(vert.u);
                    v.v = QC(vert.v);
                    v.t = QC(vert.P.y * invh);
                    v.mid = val;
                }
            }
        }

        XR_ASSERT(verts.size() == dwVerts);
        XR_ASSERT(xr::hr(dx10BufferUtils::CreateVertexBuffer(&hw_VB, verts.data(), xr::size_bytes(verts))));
        HW.stats_manager.increment_stats_vb(hw_VB);
    }

    // Fill IB
    xr_vector<u16> indices;
    indices.reserve(dwIndices);

    for (const auto dp : objects)
    {
        const auto verts = dp->vertices.size();
        const auto total = XR_ASSERT_VAL(verts * hw_BatchSize <= std::numeric_limits<u16>::max() + 1, "", verts, hw_BatchSize);

        for (std::size_t offset{0}; offset < total; offset += verts)
        {
            for (auto ind : dp->indices)
                indices.emplace_back(gsl::narrow<u16>(ind + offset));
        }
    }

    XR_ASSERT(indices.size() == dwIndices);
    XR_ASSERT(xr::hr(dx10BufferUtils::CreateIndexBuffer(&hw_IB, indices.data(), xr::size_bytes(indices))));
    HW.stats_manager.increment_stats_ib(hw_IB);

    // Declare geometry
    hw_Geom.create(dwDecl, hw_VB, hw_IB);
    XR_ASSERT(hw_Geom.stride() == sizeof(vertHW));
}

void CDetailManager::hw_Unload()
{
    // Destroy VS/VB/IB
    hw_Geom.destroy();

    HW.stats_manager.decrement_stats_vb(hw_VB);
    HW.stats_manager.decrement_stats_ib(hw_IB);

    _RELEASE(hw_IB);
    _RELEASE(hw_VB);
}
