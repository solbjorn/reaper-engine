#ifndef dx10BufferUtils_included
#define dx10BufferUtils_included

namespace dx10BufferUtils
{
[[nodiscard]] HRESULT CreateVertexBuffer(ID3DVertexBuffer** ppBuffer, const void* pData, UINT DataSize);
[[nodiscard]] HRESULT CreateIndexBuffer(ID3DIndexBuffer** ppBuffer, const void* pData, UINT DataSize);
[[nodiscard]] HRESULT CreateConstantBuffer(ID3DBuffer** ppBuffer, UINT DataSize);

void ConvertVertexDeclaration(std::span<const D3DVERTEXELEMENT9> declIn, xr_vector<D3D_INPUT_ELEMENT_DESC>& declOut);
} // namespace dx10BufferUtils

#endif //	dx10BufferUtils_included
