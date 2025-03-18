#include "stdafx.h"

#include "../xrRender/r_constants_cache.h"

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_PixelBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_pixel_cb_index_mask) >> RC_dest_pixel_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(cmd_list().m_aPixelConstants[iBufferIndex]);
    return *cmd_list().m_aPixelConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_VertexBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_vertex_cb_index_mask) >> RC_dest_vertex_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(cmd_list().m_aVertexConstants[iBufferIndex]);
    return *cmd_list().m_aVertexConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_GeometryBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_geometry_cb_index_mask) >> RC_dest_geometry_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(cmd_list().m_aGeometryConstants[iBufferIndex]);
    return *cmd_list().m_aGeometryConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_ComputeBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_compute_cb_index_mask) >> RC_dest_compute_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(cmd_list().m_aComputeConstants[iBufferIndex]);
    return *cmd_list().m_aComputeConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_HullBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_hull_cb_index_mask) >> RC_dest_hull_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(cmd_list().m_aHullConstants[iBufferIndex]);
    return *cmd_list().m_aHullConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_DomainBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_domain_cb_index_mask) >> RC_dest_domain_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(cmd_list().m_aDomainConstants[iBufferIndex]);
    return *cmd_list().m_aDomainConstants[iBufferIndex];
}

void R_constants::flush_cache()
{
    auto& cl = cmd_list();
    const auto context_id = cl.context_id;

    for (int i = 0; i < CBackend::MaxCBuffers; ++i)
    {
        if (cl.m_aVertexConstants[i])
            cl.m_aVertexConstants[i]->Flush(context_id);

        if (cl.m_aPixelConstants[i])
            cl.m_aPixelConstants[i]->Flush(context_id);

        if (cl.m_aGeometryConstants[i])
            cl.m_aGeometryConstants[i]->Flush(context_id);

        if (cl.m_aHullConstants[i])
            cl.m_aHullConstants[i]->Flush(context_id);

        if (cl.m_aDomainConstants[i])
            cl.m_aDomainConstants[i]->Flush(context_id);

        if (cl.m_aComputeConstants[i])
            cl.m_aComputeConstants[i]->Flush(context_id);
    }
}
