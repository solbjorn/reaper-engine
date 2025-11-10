#include "stdafx.h"

// Create Quad-IB

namespace
{
class IndexBufferGen
{
    static constexpr gsl::index dwTriCount{4 * 1024};
    static constexpr gsl::index dwIdxCount{dwTriCount * 2 * 3};

    std::array<u16, dwIdxCount> XR_ALIGNED_DEFAULT Indices;

public:
    constexpr IndexBufferGen()
    {
        gsl::index ICnt{};
        u16 Cnt{};

        for (gsl::index i{}; i < dwTriCount; ++i)
        {
            Indices[ICnt++] = Cnt;
            Indices[ICnt++] = Cnt + 1;
            Indices[ICnt++] = Cnt + 2;

            Indices[ICnt++] = Cnt + 3;
            Indices[ICnt++] = Cnt + 2;
            Indices[ICnt++] = Cnt + 1;

            Cnt += 4;
        }
    }

    constexpr const u16* data() const { return Indices.data(); }
    constexpr gsl::index width() const { return dwIdxCount * sizeof(u16); }
};

constexpr IndexBufferGen IndexBuffer;
} // namespace

void dxRenderDeviceRender::CreateQuadIB()
{
    D3D_BUFFER_DESC desc{};
    desc.ByteWidth = IndexBuffer.width();
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.BindFlags = D3D_BIND_INDEX_BUFFER;

    D3D_SUBRESOURCE_DATA subData{};
    subData.pSysMem = IndexBuffer.data();

    R_CHK(HW.pDevice->CreateBuffer(&desc, &subData, &QuadIB));
    HW.stats_manager.increment_stats_ib(QuadIB);
}
