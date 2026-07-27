#include "stdafx.h"

#include "HWCaps.h"

constexpr u32 GetGpuNum() { return 1; }

void CHWCaps::Update()
{
    // ***************** GEOMETRY
    geometry_major = 5;
    geometry_minor = 0;
    geometry.bSoftware = FALSE;
    geometry.bPointSprites = FALSE;
    geometry.bNPatches = FALSE;
    DWORD cnt = 256;
    clamp<DWORD>(cnt, 0, 256);
    geometry.dwRegisters = cnt;
    geometry.dwInstructions = 256;
    geometry.dwClipPlanes = _min(6, 15);
    geometry.bVTF = TRUE;

    // ***************** PIXEL processing
    raster_major = 5;
    raster_minor = 0;
    raster.dwStages = 15;
    raster.bNonPow2 = TRUE;
    raster.bCubemap = TRUE;
    raster.dwMRT_count = 4;
    raster.b_MRT_mixdepth = TRUE;
    raster.dwInstructions = 256;

    // *******1********** Vertex cache
    //	TODO: DX10: Find a way to detect cache size
    geometry.dwVertexCache = 24;

    //
    bTableFog = FALSE; // BOOL	(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);

    // Detect if stencil available
    bStencil = TRUE;

    // Scissoring
    bScissor = TRUE;

    // Stencil relative caps
    soInc = D3DSTENCILOP_INCRSAT;
    soDec = D3DSTENCILOP_DECRSAT;
    dwMaxStencilValue = (1 << 8) - 1;

    // DEV INFO

    iGPUNum = GetGpuNum();
}
