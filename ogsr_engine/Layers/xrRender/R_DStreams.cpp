#include "stdafx.h"

#include "R_DStreams.h"

#include "FLOD.h"
#include "ResourceManager.h"

namespace xr
{
namespace
{
[[nodiscard]] constexpr std::size_t dvb_size(ctx_id_t context_id) { return (context_id == R__IMM_CTX_ID ? 16 : 4) * 1024 * 1024; }
[[nodiscard]] constexpr std::size_t dib_size(ctx_id_t context_id) { return (context_id == R__IMM_CTX_ID ? 4 : 1) * 1024 * 1024; }

// Make sure the immediate vertex buffer can handle the entire 16-bit Quad IB (65536 LOD verts)
static_assert(xr::dvb_size(R__IMM_CTX_ID) >= (std::numeric_limits<u16>::max() + 1) * sizeof(FLOD::_hw));
// Make sure a deferred vertex buffer can handle the entire 16-bit Quad IB (65536 TL verts)
static_assert(xr::dvb_size(R__PARALLEL_CTX_ID) >= (std::numeric_limits<u16>::max() + 1) * sizeof(FVF::TL));
} // namespace
} // namespace xr

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void _VertexStream::Create(ctx_id_t context_id)
{
    this->context_id = context_id;
    mPosition = 0;
    cache.resize(xr::dvb_size(context_id));

    D3D_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = cache.size();
    bufferDesc.Usage = D3D_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;

    XR_ASSERT(xr::hr(HW.pDevice->CreateBuffer(&bufferDesc, nullptr, &pVB)));
    HW.stats_manager.increment_stats_vb(Buffer());
}

void _VertexStream::Destroy()
{
    if (!pVB)
        return;

    HW.stats_manager.decrement_stats_vb(Buffer());
    _clear();
}

void* _VertexStream::Lock(std::size_t count, std::size_t stride)
{
    // Ensure there is enough space in the VB for this data
    XR_ASSERT(count > 0 && count * stride <= cache.size(), "vertex buffer overflow", count, stride, cache.size());

    return static_cast<void*>(cache.data());
}

std::size_t _VertexStream::Unlock(std::size_t count, std::size_t stride)
{
    const auto len = count * stride;
    const auto size = cache.size();

    XR_ASSERT(len <= size, "vertex buffer overflow", len, count, stride, size);

    // Vertex-local info
    const auto vl_size = size / stride;
    const auto vl_pos = std::__div_ceil(mPosition, stride);

    // Check if there is need to flush and perform lock
    std::size_t vl_off;
    D3D_MAP map;

    if (vl_pos + count > vl_size)
    {
        vl_off = 0;
        map = D3D_MAP_WRITE_DISCARD;
    }
    else
    {
        vl_off = vl_pos;
        map = D3D_MAP_WRITE_NO_OVERWRITE;
    }

    mPosition = vl_off * stride;

    D3D11_MAPPED_SUBRESOURCE res;
    const auto context = HW.get_context(context_id);

    XR_ASSERT(xr::hr(context->Map(Buffer(), 0, map, 0, &res)));
    xr_memcpy(static_cast<std::byte*>(res.pData) + mPosition, std::assume_aligned<64>(cache.data()), len);
    context->Unmap(Buffer(), 0);

    mPosition += len;

    return vl_off;
}

//////////////////////////////////////////////////////////////////////////

void _IndexStream::Create(ctx_id_t context_id)
{
    this->context_id = context_id;
    mPosition = 0;
    cache.resize(xr::dib_size(context_id));

    D3D_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = cache.size();
    bufferDesc.Usage = D3D_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;

    XR_ASSERT(xr::hr(HW.pDevice->CreateBuffer(&bufferDesc, nullptr, &pIB)));
    HW.stats_manager.increment_stats_ib(Buffer());
}

void _IndexStream::Destroy()
{
    if (!pIB)
        return;

    HW.stats_manager.decrement_stats_ib(Buffer());
    _clear();
}

u16* _IndexStream::lock_raw(std::size_t count)
{
    // Ensure there is enough space in the IB for this data
    XR_ASSERT(count > 0 && count * stride <= cache.size(), "index buffer overflow", count, stride, cache.size());

    return reinterpret_cast<u16*>(cache.data());
}

std::size_t _IndexStream::unlock_raw(std::size_t count)
{
    const auto len = count * stride;
    const auto size = cache.size();

    XR_ASSERT(len <= size, "index buffer overflow", len, count, stride, size);

    // Index-local info
    const auto il_size = size / stride;
    const auto il_pos = mPosition / stride;

    // Check if there is need to flush and perform lock
    std::size_t il_off;
    D3D_MAP map;

    if (il_pos + count > il_size)
    {
        il_off = 0;
        map = D3D_MAP_WRITE_DISCARD;
    }
    else
    {
        il_off = il_pos;
        map = D3D_MAP_WRITE_NO_OVERWRITE;
    }

    mPosition = il_off * stride;

    D3D11_MAPPED_SUBRESOURCE res;
    const auto context = HW.get_context(context_id);

    XR_ASSERT(xr::hr(context->Map(Buffer(), 0, map, 0, &res)));
    xr_memcpy(static_cast<std::byte*>(res.pData) + mPosition, std::assume_aligned<64>(cache.data()), len);
    context->Unmap(Buffer(), 0);

    mPosition += len;

    return il_off;
}
