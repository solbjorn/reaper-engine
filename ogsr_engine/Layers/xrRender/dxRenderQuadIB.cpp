#include "stdafx.h"

// Create Quad-IB

namespace
{
class IndexBufferGen final
{
private:
    // 16-bit Quad IB can index up to 65536 vertices, each quad needs 4 vertices
    static constexpr auto quad_count = (std::numeric_limits<u16>::max() + 1uz) / 4;
    // Each quad consists of 2 triangles
    static constexpr auto tri_count = quad_count * 2;
    // Each triangle requires 3 indices
    static constexpr auto idx_count = tri_count * 3;

    static_assert(idx_count == 98304);

    std::array<u16, idx_count> XR_ALIGNED_DEFAULT indices;

public:
    constexpr IndexBufferGen()
    {
        for (auto [quad, ind] : std::views::enumerate(indices | std::views::chunk(6)))
        {
            for (auto [i, add] : std::views::zip(ind, std::array{0, 1, 2, 3, 2, 1}))
                i = gsl::narrow_cast<u16>(quad * 4 + add);
        }
    }

    [[nodiscard]] constexpr auto data() const { return indices.data(); }
    [[nodiscard]] constexpr auto max() const { return std::ranges::max(indices); }
    [[nodiscard]] constexpr auto width() const { return xr::size_bytes(indices); }
};

constexpr IndexBufferGen IndexBuffer;
static_assert(IndexBuffer.max() == std::numeric_limits<u16>::max());
} // namespace

void dxRenderDeviceRender::CreateQuadIB()
{
    D3D_BUFFER_DESC desc{};
    desc.ByteWidth = IndexBuffer.width();
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.BindFlags = D3D_BIND_INDEX_BUFFER;

    D3D_SUBRESOURCE_DATA subData{};
    subData.pSysMem = IndexBuffer.data();

    XR_ASSERT(xr::hr(HW.pDevice->CreateBuffer(&desc, &subData, &QuadIB)));
    HW.stats_manager.increment_stats_ib(QuadIB);
}
