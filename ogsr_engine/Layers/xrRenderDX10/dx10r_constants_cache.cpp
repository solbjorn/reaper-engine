#include "stdafx.h"

#include "../xrRender/r_constants_cache.h"

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_PixelBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_pixel_cb_index_mask) >> RC_dest_pixel_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(RCache.m_aPixelConstants[iBufferIndex]);
    return *RCache.m_aPixelConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_VertexBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_vertex_cb_index_mask) >> RC_dest_vertex_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(RCache.m_aVertexConstants[iBufferIndex]);
    return *RCache.m_aVertexConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_GeometryBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_geometry_cb_index_mask) >> RC_dest_geometry_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(RCache.m_aGeometryConstants[iBufferIndex]);
    return *RCache.m_aGeometryConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_ComputeBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_compute_cb_index_mask) >> RC_dest_compute_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(RCache.m_aComputeConstants[iBufferIndex]);
    return *RCache.m_aComputeConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_HullBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_hull_cb_index_mask) >> RC_dest_hull_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(RCache.m_aHullConstants[iBufferIndex]);
    return *RCache.m_aHullConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_DomainBuffer>(R_constant* C) const
{
    //	Decode index
    int iBufferIndex = (C->destination & RC_dest_domain_cb_index_mask) >> RC_dest_domain_cb_index_shift;

    VERIFY(iBufferIndex < CBackend::MaxCBuffers);
    VERIFY(RCache.m_aDomainConstants[iBufferIndex]);
    return *RCache.m_aDomainConstants[iBufferIndex];
}

void R_constants::flush_cache()
{
    for (int i = 0; i < CBackend::MaxCBuffers; ++i)
    {
        if (RCache.m_aVertexConstants[i])
            RCache.m_aVertexConstants[i]->Flush();

        if (RCache.m_aPixelConstants[i])
            RCache.m_aPixelConstants[i]->Flush();

        if (RCache.m_aGeometryConstants[i])
            RCache.m_aGeometryConstants[i]->Flush();

        if (RCache.m_aHullConstants[i])
            RCache.m_aHullConstants[i]->Flush();

        if (RCache.m_aDomainConstants[i])
            RCache.m_aDomainConstants[i]->Flush();

        if (RCache.m_aComputeConstants[i])
            RCache.m_aComputeConstants[i]->Flush();
    }
}
