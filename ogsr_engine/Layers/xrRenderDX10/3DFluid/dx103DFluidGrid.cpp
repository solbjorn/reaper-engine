#include "stdafx.h"
#include "dx103DFluidGrid.h"
#include "../dx10BufferUtils.h"
#include <Utilities\FlexibleVertexFormat.h>

struct VS_INPUT_FLUIDSIM_STRUCT
{
    Fvector3 Pos; // Clip space position for slice vertices
    Fvector3 Tex; // Cell coordinates in 0-"texture dimension" range
};

inline void ComputeRowColsForFlat3DTexture(int depth, int* outCols, int* outRows)
{
    // Compute # of m_iRows and m_iCols for a "flat 3D-texture" configuration
    // (in this configuration all the slices in the volume are spread in a single 2D texture)
    int m_iRows = (int)floorf(_sqrt((float)depth));
    int m_iCols = m_iRows;
    while (m_iRows * m_iCols < depth)
    {
        m_iCols++;
    }
    VERIFY(m_iRows * m_iCols >= depth);

    *outCols = m_iCols;
    *outRows = m_iRows;
}

#define VERTICES_PER_SLICE 6
#define VERTICES_PER_LINE 2
#define LINES_PER_SLICE 4

dx103DFluidGrid::dx103DFluidGrid() {}
dx103DFluidGrid::~dx103DFluidGrid() { DestroyVertexBuffers(); }

void dx103DFluidGrid::Initialize(int gridWidth, int gridHeight, int gridDepth)
{
    m_vDim[0] = gridWidth;
    m_vDim[1] = gridHeight;
    m_vDim[2] = gridDepth;

    m_iMaxDim = _max(_max(m_vDim[0], m_vDim[1]), m_vDim[2]);

    ComputeRowColsForFlat3DTexture(m_vDim[2], &m_iCols, &m_iRows);

    CreateVertexBuffers();
}

void dx103DFluidGrid::CreateVertexBuffers()
{
    static constexpr D3DVERTEXELEMENT9 layoutDesc[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, D3DDECL_END()};

    u32 vSize = FVF::ComputeVertexSize(layoutDesc, 0);

    m_iNumVerticesRenderQuad = VERTICES_PER_SLICE * m_vDim[2];
    auto* renderQuad = xr_alloc<VS_INPUT_FLUIDSIM_STRUCT>(m_iNumVerticesRenderQuad);

    m_iNumVerticesSlices = VERTICES_PER_SLICE * (m_vDim[2] - 2);
    auto* slices = xr_alloc<VS_INPUT_FLUIDSIM_STRUCT>(m_iNumVerticesSlices);

    m_iNumVerticesBoundarySlices = VERTICES_PER_SLICE * 2;
    auto* boundarySlices = xr_alloc<VS_INPUT_FLUIDSIM_STRUCT>(m_iNumVerticesBoundarySlices);

    m_iNumVerticesBoundaryLines = VERTICES_PER_LINE * LINES_PER_SLICE * (m_vDim[2]);
    auto* boundaryLines = xr_alloc<VS_INPUT_FLUIDSIM_STRUCT>(m_iNumVerticesBoundaryLines);

    VERIFY(renderQuad && m_iNumVerticesSlices && m_iNumVerticesBoundarySlices && m_iNumVerticesBoundaryLines);

    // Vertex buffer for "m_vDim[2]" quads to draw all the slices of the 3D-texture as a flat 3D-texture
    // (used to draw all the individual slices at once to the screen buffer)
    int index{};
    for (int z = 0; z < m_vDim[2]; z++)
        InitScreenSlice(&renderQuad, z, index);

    CHK_DX(dx10BufferUtils::CreateVertexBuffer(&m_pRenderQuadBuffer, renderQuad, vSize * m_iNumVerticesRenderQuad));
    m_GeomRenderQuad.create(layoutDesc, m_pRenderQuadBuffer, 0);

    // Vertex buffer for "m_vDim[2]" quads to draw all the slices to a 3D texture
    // (a Geometry Shader is used to send each quad to the appropriate slice)
    index = 0;
    for (int z = 1; z < m_vDim[2] - 1; z++)
        InitSlice(z, &slices, index);
    VERIFY(index == m_iNumVerticesSlices);

    CHK_DX(dx10BufferUtils::CreateVertexBuffer(&m_pSlicesBuffer, slices, vSize * m_iNumVerticesSlices));
    m_GeomSlices.create(layoutDesc, m_pSlicesBuffer, 0);

    // Vertex buffers for boundary geometry
    //   2 boundary slices
    index = 0;
    InitBoundaryQuads(&boundarySlices, index);
    VERIFY(index == m_iNumVerticesBoundarySlices);

    CHK_DX(dx10BufferUtils::CreateVertexBuffer(&m_pBoundarySlicesBuffer, boundarySlices, vSize * m_iNumVerticesBoundarySlices));
    m_GeomBoundarySlices.create(layoutDesc, m_pBoundarySlicesBuffer, 0);

    //   ( 4 * "m_vDim[2]" ) boundary lines
    index = 0;
    InitBoundaryLines(&boundaryLines, index);
    VERIFY(index == m_iNumVerticesBoundaryLines);

    CHK_DX(dx10BufferUtils::CreateVertexBuffer(&m_pBoundaryLinesBuffer, boundaryLines, vSize * m_iNumVerticesBoundaryLines));
    m_GeomBoundaryLines.create(layoutDesc, m_pBoundaryLinesBuffer, 0);

    // cleanup:
    xr_free(renderQuad);
    xr_free(slices);
    xr_free(boundarySlices);
    xr_free(boundaryLines);
}

void dx103DFluidGrid::DestroyVertexBuffers()
{
    _RELEASE(m_pRenderQuadBuffer);
    _RELEASE(m_pSlicesBuffer);
    _RELEASE(m_pBoundarySlicesBuffer);
    _RELEASE(m_pBoundaryLinesBuffer);
}

void dx103DFluidGrid::InitScreenSlice(VS_INPUT_FLUIDSIM_STRUCT** vertices, int z, int& index)
{
    VS_INPUT_FLUIDSIM_STRUCT tempVertex1;
    VS_INPUT_FLUIDSIM_STRUCT tempVertex2;
    VS_INPUT_FLUIDSIM_STRUCT tempVertex3;
    VS_INPUT_FLUIDSIM_STRUCT tempVertex4;

    // compute the offset (px, py) in the "flat 3D-texture" space for the slice with given 'z' coordinate
    const int column = z % m_iCols;
    const int row = (int)floorf((float)(z / m_iCols));
    const int px = column * m_vDim[0];
    const int py = row * m_vDim[1];

    const float w = float(m_vDim[0]);
    const float h = float(m_vDim[1]);

    const float Width = float(m_iCols * m_vDim[0]);
    const float Height = float(m_iRows * m_vDim[1]);

    tempVertex1.Pos.set(px * 2.0f / Width - 1.0f, -(py * 2.0f / Height) + 1.0f, 0.0f);
    tempVertex1.Tex.set(0, 0, float(z));

    tempVertex2.Pos.set((px + w) * 2.0f / Width - 1.0f, -((py) * 2.0f / Height) + 1.0f, 0.0f);
    tempVertex2.Tex.set(w, 0, float(z));

    tempVertex3.Pos.set((px + w) * 2.0f / Width - 1.0f, -((py + h) * 2.0f / Height) + 1.0f, 0.0f);
    tempVertex3.Tex.set(w, h, float(z));

    tempVertex4.Pos.set((px) * 2.0f / Width - 1.0f, -((py + h) * 2.0f / Height) + 1.0f, 0.0f);
    tempVertex4.Tex.set(0, h, float(z));

    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex2;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex4;
}

void dx103DFluidGrid::InitSlice(int z, VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index)
{
    VS_INPUT_FLUIDSIM_STRUCT tempVertex1;
    VS_INPUT_FLUIDSIM_STRUCT tempVertex2;
    VS_INPUT_FLUIDSIM_STRUCT tempVertex3;
    VS_INPUT_FLUIDSIM_STRUCT tempVertex4;

    const int w = m_vDim[0];
    const int h = m_vDim[1];

    tempVertex1.Pos.set(1 * 2.0f / w - 1.0f, -1 * 2.0f / h + 1.0f, 0.0f);
    tempVertex1.Tex.set(1.0f, 1.0f, float(z));

    tempVertex2.Pos.set((w - 1.0f) * 2.0f / w - 1.0f, -1 * 2.0f / h + 1.0f, 0.0f);
    tempVertex2.Tex.set((w - 1.0f), 1.0f, float(z));

    tempVertex3.Pos.set((w - 1.0f) * 2.0f / w - 1.0f, -(h - 1) * 2.0f / h + 1.0f, 0.0f);
    tempVertex3.Tex.set((w - 1.0f), (h - 1.0f), float(z));

    tempVertex4.Pos.set(1 * 2.0f / w - 1.0f, -(h - 1.0f) * 2.0f / h + 1.0f, 0.0f);
    tempVertex4.Tex.set(1.0f, (h - 1.0f), float(z));

    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex2;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex4;
}

void dx103DFluidGrid::InitLine(float x1, float y1, float x2, float y2, int z, VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index)
{
    VS_INPUT_FLUIDSIM_STRUCT tempVertex;
    int w = m_vDim[0];
    int h = m_vDim[1];

    tempVertex.Pos.set(x1 * 2.0f / w - 1.0f, -y1 * 2.0f / h + 1.0f, 0.5f);
    tempVertex.Tex.set(0.0f, 0.0f, float(z));
    (*vertices)[index++] = tempVertex;

    tempVertex.Pos.set(x2 * 2.0f / w - 1.0f, -y2 * 2.0f / h + 1.0f, 0.5f);
    tempVertex.Tex.set(0.0f, 0.0f, float(z));
    (*vertices)[index++] = tempVertex;
}

void dx103DFluidGrid::InitBoundaryQuads(VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index)
{
    InitSlice(0, vertices, index);
    InitSlice(m_vDim[2] - 1, vertices, index);
}

void dx103DFluidGrid::InitBoundaryLines(VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index)
{
    int w = m_vDim[0];
    int h = m_vDim[1];

    for (int z = 0; z < m_vDim[2]; z++)
    {
        // bottom
        InitLine(0.0f, 1.0f, float(w), 1.0f, z, vertices, index);
        // top
        InitLine(0.0f, float(h), float(w), float(h), z, vertices, index);
        // left
        InitLine(1.0f, 0.0f, 1.0f, float(h), z, vertices, index);
        // right
        InitLine(float(w), 0.0f, float(w), float(h), z, vertices, index);
    }
}

void dx103DFluidGrid::DrawSlices(void)
{
    RCache.set_Geometry(m_GeomSlices);
    RCache.Render(D3DPT_TRIANGLELIST, 0, m_iNumVerticesSlices / 3);
}

void dx103DFluidGrid::DrawSlicesToScreen(void)
{
    RCache.set_Geometry(m_GeomRenderQuad);
    RCache.Render(D3DPT_TRIANGLELIST, 0, m_iNumVerticesRenderQuad / 3);
}

void dx103DFluidGrid::DrawBoundaryQuads(void)
{
    RCache.set_Geometry(m_GeomBoundarySlices);
    RCache.Render(D3DPT_TRIANGLELIST, 0, m_iNumVerticesBoundarySlices / 3);
}

void dx103DFluidGrid::DrawBoundaryLines(void)
{
    RCache.set_Geometry(m_GeomBoundaryLines);
    RCache.Render(D3DPT_TRIANGLELIST, 0, m_iNumVerticesBoundaryLines / 3);
}
