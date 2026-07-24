#include "stdafx.h"

#include "../xrRender/r_constants_cache.h"

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_PixelBuffer>(R_constant* C) const
{
    //	Decode index
    const size_t iBufferIndex =
        XR_ASSERT_VAL(((C->destination & RC_dest_pixel_cb_index_mask) >> RC_dest_pixel_cb_index_shift) < CBackend::MaxCBuffers, "", C->destination);

    return *XR_ASSERT_VAL(cmd_list().m_aPixelConstants[iBufferIndex]);
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_VertexBuffer>(R_constant* C) const
{
    //	Decode index
    const size_t iBufferIndex =
        XR_ASSERT_VAL(((C->destination & RC_dest_vertex_cb_index_mask) >> RC_dest_vertex_cb_index_shift) < CBackend::MaxCBuffers, "", C->destination);

    return *XR_ASSERT_VAL(cmd_list().m_aVertexConstants[iBufferIndex]);
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_GeometryBuffer>(R_constant* C) const
{
    //	Decode index
    const size_t iBufferIndex =
        XR_ASSERT_VAL(((C->destination & RC_dest_geometry_cb_index_mask) >> RC_dest_geometry_cb_index_shift) < CBackend::MaxCBuffers, "", C->destination);

    return *XR_ASSERT_VAL(cmd_list().m_aGeometryConstants[iBufferIndex]);
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_ComputeBuffer>(R_constant* C) const
{
    //	Decode index
    const size_t iBufferIndex =
        XR_ASSERT_VAL(((C->destination & RC_dest_compute_cb_index_mask) >> RC_dest_compute_cb_index_shift) < CBackend::MaxCBuffers, "", C->destination);

    return *XR_ASSERT_VAL(cmd_list().m_aComputeConstants[iBufferIndex]);
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_HullBuffer>(R_constant* C) const
{
    //	Decode index
    const size_t iBufferIndex =
        XR_ASSERT_VAL(((C->destination & RC_dest_hull_cb_index_mask) >> RC_dest_hull_cb_index_shift) < CBackend::MaxCBuffers, "", C->destination);

    return *XR_ASSERT_VAL(cmd_list().m_aHullConstants[iBufferIndex]);
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_DomainBuffer>(R_constant* C) const
{
    //	Decode index
    const size_t iBufferIndex =
        XR_ASSERT_VAL(((C->destination & RC_dest_domain_cb_index_mask) >> RC_dest_domain_cb_index_shift) < CBackend::MaxCBuffers, "", C->destination);

    return *XR_ASSERT_VAL(cmd_list().m_aDomainConstants[iBufferIndex]);
}

void R_constants::flush_cache()
{
    auto& cl = cmd_list();
    const auto context_id = cl.context_id;

    for (auto [vc, pc, gc, hc, dc, cc] : std::views::zip(cl.m_aVertexConstants, cl.m_aPixelConstants, cl.m_aGeometryConstants, cl.m_aHullConstants,
                                                         cl.m_aDomainConstants, cl.m_aComputeConstants))
    {
        if (vc)
            vc->Flush(context_id);
        if (pc)
            pc->Flush(context_id);
        if (gc)
            gc->Flush(context_id);
        if (hc)
            hc->Flush(context_id);
        if (dc)
            dc->Flush(context_id);
        if (cc)
            cc->Flush(context_id);
    }
}
