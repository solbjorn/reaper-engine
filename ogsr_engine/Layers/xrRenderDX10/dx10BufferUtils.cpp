#include "stdafx.h"

#include "dx10BufferUtils.h"

namespace dx10BufferUtils
{
namespace
{
[[nodiscard]] inline HRESULT CreateBuffer(ID3DBuffer** ppBuffer, const void* pData, UINT DataSize, u32 flags)
{
    D3D_BUFFER_DESC desc{};
    desc.ByteWidth = DataSize;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.BindFlags = flags;

    D3D_SUBRESOURCE_DATA subData{};
    subData.pSysMem = pData;

    return HW.pDevice->CreateBuffer(&desc, &subData, ppBuffer);
}
} // namespace

HRESULT CreateVertexBuffer(ID3DVertexBuffer** ppBuffer, const void* pData, UINT DataSize)
{
    return CreateBuffer(ppBuffer, pData, DataSize, D3D_BIND_VERTEX_BUFFER);
}

HRESULT CreateIndexBuffer(ID3DIndexBuffer** ppBuffer, const void* pData, UINT DataSize)
{
    return CreateBuffer(ppBuffer, pData, DataSize, D3D_BIND_INDEX_BUFFER);
}

HRESULT CreateConstantBuffer(ID3DBuffer** ppBuffer, UINT DataSize)
{
    D3D_BUFFER_DESC desc{};
    desc.ByteWidth = DataSize;
    desc.Usage = D3D_USAGE_DYNAMIC;
    desc.BindFlags = D3D_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;

    return HW.pDevice->CreateBuffer(&desc, nullptr, ppBuffer);
}

namespace
{
constexpr std::array<std::pair<D3DDECLTYPE, DXGI_FORMAT>, 15> VertexFormatList{{
    {D3DDECLTYPE_FLOAT1, DXGI_FORMAT_R32_FLOAT},
    {D3DDECLTYPE_FLOAT2, DXGI_FORMAT_R32G32_FLOAT},
    {D3DDECLTYPE_FLOAT3, DXGI_FORMAT_R32G32B32_FLOAT},
    {D3DDECLTYPE_FLOAT4, DXGI_FORMAT_R32G32B32A32_FLOAT},
    // Warning. Explicit RGB component swizzling is nesessary
    {D3DDECLTYPE_D3DCOLOR, DXGI_FORMAT_R8G8B8A8_UNORM},
    // Note: Shader gets UINT values, but if Direct3D 9 style integral floats are needed (0.0f, 1.0f... 255.f),
    // UINT can just be converted to float32 in shader.
    {D3DDECLTYPE_UBYTE4, DXGI_FORMAT_R8G8B8A8_UINT},
    // Note: Shader gets SINT values, but if Direct3D 9 style integral floats are needed,
    // SINT can just be converted to float32 in shader.
    {D3DDECLTYPE_SHORT2, DXGI_FORMAT_R16G16_SINT},
    // Note: Shader gets SINT values, but if Direct3D 9 style integral floats are needed,
    // SINT can just be converted to float32 in shader.
    {D3DDECLTYPE_SHORT4, DXGI_FORMAT_R16G16B16A16_SINT},
    {D3DDECLTYPE_UBYTE4N, DXGI_FORMAT_R8G8B8A8_UNORM},
    {D3DDECLTYPE_SHORT2N, DXGI_FORMAT_R16G16_SNORM},
    {D3DDECLTYPE_SHORT4N, DXGI_FORMAT_R16G16B16A16_SNORM},
    {D3DDECLTYPE_USHORT2N, DXGI_FORMAT_R16G16_UNORM},
    {D3DDECLTYPE_USHORT4N, DXGI_FORMAT_R16G16B16A16_UNORM},
    // D3DDECLTYPE_UDEC3 Not available
    // D3DDECLTYPE_DEC3N Not available
    {D3DDECLTYPE_FLOAT16_2, DXGI_FORMAT_R16G16_FLOAT},
    {D3DDECLTYPE_FLOAT16_4, DXGI_FORMAT_R16G16B16A16_FLOAT},
}};

[[nodiscard]] DXGI_FORMAT ConvertVertexFormat(D3DDECLTYPE dx9FMT)
{
    if (const auto it = std::ranges::find(VertexFormatList, dx9FMT, &std::pair<D3DDECLTYPE, DXGI_FORMAT>::first); it != VertexFormatList.end())
        return it->second;

    XR_PANIC("no corresponding vertex format", dx9FMT);
}

constexpr std::array<std::pair<D3DDECLUSAGE, gsl::czstring>, 10> VertexSemanticList{{
    {D3DDECLUSAGE_POSITION, "POSITION"}, //	0
    {D3DDECLUSAGE_BLENDWEIGHT, "BLENDWEIGHT"}, // 1
    {D3DDECLUSAGE_BLENDINDICES, "BLENDINDICES"}, // 2
    {D3DDECLUSAGE_NORMAL, "NORMAL"}, // 3
    {D3DDECLUSAGE_PSIZE, "PSIZE"}, // 4
    {D3DDECLUSAGE_TEXCOORD, "TEXCOORD"}, // 5
    {D3DDECLUSAGE_TANGENT, "TANGENT"}, // 6
    {D3DDECLUSAGE_BINORMAL, "BINORMAL"}, // 7
    // D3DDECLUSAGE_TESSFACTOR,    // 8
    {D3DDECLUSAGE_POSITIONT, "POSITIONT"}, // 9
    {D3DDECLUSAGE_COLOR, "COLOR"}, // 10
    // D3DDECLUSAGE_FOG,           // 11
    // D3DDECLUSAGE_DEPTH,         // 12
    // D3DDECLUSAGE_SAMPLE,        // 13
}};

[[nodiscard]] gsl::czstring ConvertSemantic(D3DDECLUSAGE Semantic)
{
    if (const auto it = std::ranges::find(VertexSemanticList, Semantic, &std::pair<D3DDECLUSAGE, gsl::czstring>::first); it != VertexSemanticList.end())
        return it->second;

    XR_PANIC("no corresponding input semantic", Semantic);
}
} // namespace

void ConvertVertexDeclaration(std::span<const D3DVERTEXELEMENT9> declIn, xr_vector<D3D_INPUT_ELEMENT_DESC>& declOut)
{
    declOut.reserve(XR_ASSERT_VAL(declIn.size() > 0));

    declOut.assign_range(declIn | std::views::take_while([] [[nodiscard]] (auto& desc) { return desc.Stream != std::numeric_limits<u8>::max(); }) |
                         std::views::transform([] [[nodiscard]] (auto& desc) {
                             return D3D_INPUT_ELEMENT_DESC{
                                 .SemanticName = ConvertSemantic((D3DDECLUSAGE)desc.Usage),
                                 .SemanticIndex = desc.UsageIndex,
                                 .Format = ConvertVertexFormat((D3DDECLTYPE)desc.Type),
                                 .InputSlot = desc.Stream,
                                 .AlignedByteOffset = desc.Offset,
                                 .InputSlotClass = D3D_INPUT_PER_VERTEX_DATA,
                                 .InstanceDataStepRate = 0,
                             };
                         }));

    declOut.push_back({});
    XR_ASSERT(declOut.size() == declIn.size());
}
} // namespace dx10BufferUtils
