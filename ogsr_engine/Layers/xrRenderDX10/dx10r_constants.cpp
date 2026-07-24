#include "stdafx.h"

#include "../xrRender/r_constants.h"

#include "../xrRender/ResourceManager.h"
#include "../xrRenderDX10/dx10ConstantBuffer.h"

#include "../../xrCore/xrPool.h"

BOOL R_constant_table::parseConstants(ID3DShaderReflectionConstantBuffer* pTable, u32 destination)
{
    XR_ASSERT(pTable != nullptr, "", destination);

    D3D_SHADER_BUFFER_DESC TableDesc{};
    XR_ASSERT(xr::hr(pTable->GetDesc(&TableDesc)));

    for (u32 i = 0; i < TableDesc.Variables; ++i)
    {
        auto pVar = XR_ASSERT_VAL(pTable->GetVariableByIndex(i) != nullptr);
        D3D_SHADER_VARIABLE_DESC VarDesc{};
        pVar->GetDesc(&VarDesc);

        D3D_SHADER_TYPE_DESC TypeDesc{};
        XR_ASSERT_VAL(pVar->GetType() != nullptr)->GetDesc(&TypeDesc);

        // Name
        LPCSTR name = VarDesc.Name;

        // Type
        u16 type = u16(-1);
        switch (TypeDesc.Type)
        {
        case D3D10_SVT_FLOAT: type = RC_float; break;
        case D3D10_SVT_BOOL: type = RC_bool; break;
        case D3D10_SVT_INT: type = RC_int; break;
        default: XR_PANIC("unexpected shader constant type", name, TypeDesc.Type);
        }

        // Rindex,Rcount
        //	Used as byte offset in constant buffer
        u16 r_index = XR_ASSERT_VAL(VarDesc.StartOffset < std::numeric_limits<u16>::max());
        u16 r_type = u16(-1);

        // TypeInfo + class
        switch (TypeDesc.Class)
        {
        case D3D10_SVC_SCALAR: r_type = RC_1x1; break;
        case D3D10_SVC_VECTOR: {
            switch (TypeDesc.Columns)
            {
            case 4: r_type = RC_1x4; break;
            case 3: r_type = RC_1x3; break;
            case 2: r_type = RC_1x2; break;
            default: XR_PANIC("unsupported vector constant dimension", name, TypeDesc.Columns);
            }
        }
        break;
        case D3D10_SVC_MATRIX_ROWS: {
            switch (TypeDesc.Columns)
            {
            case 4:
                switch (TypeDesc.Rows)
                {
                case 2: r_type = RC_2x4; break;
                case 3: r_type = RC_3x4; break;
                case 4: r_type = RC_4x4; break;
                default: XR_PANIC("unsupported matrix constant row number", name, TypeDesc.Rows);
                }
                break;
            default: XR_PANIC("unsupported matrix constant column number", name, TypeDesc.Columns);
            }
        }
        break;
        case D3D10_SVC_MATRIX_COLUMNS:
        case D3D10_SVC_STRUCT:
        case D3D10_SVC_OBJECT: XR_PANIC("constant class is not supported", name, TypeDesc.Class);
        default: continue;
        }

        // We have determined all valuable info, search if constant already created
        ref_constant C = get(name);
        if (!C)
        {
            C = table.emplace_back(xr_new<R_constant>()); //.g_constant_allocator.create();
            C->name._set(name);
            C->destination = destination;
            C->type = type;
            R_constant_load& L = C->get_load(destination);
            L.index = r_index;
            L.cls = r_type;
        }
        else
        {
            C->destination |= destination;
            XR_ASSERT(C->type == type);
            R_constant_load& L = C->get_load(destination);
            L.index = r_index;
            L.cls = r_type;
        }
    }

    return TRUE;
}

BOOL R_constant_table::parseResources(ID3DShaderReflection* pReflection, int ResNum, u32 destination)
{
    for (int i = 0; i < ResNum; ++i)
    {
        D3D_SHADER_INPUT_BIND_DESC ResDesc{};
        pReflection->GetResourceBindingDesc(i, &ResDesc);
        XR_ASSERT(ResDesc.BindCount == 1, "", i);

        u16 type;
        switch (ResDesc.Type)
        {
        case D3D10_SIT_TEXTURE: type = RC_dx10texture; break;
        case D3D10_SIT_SAMPLER: type = RC_sampler; break;
        case D3D11_SIT_UAV_RWTYPED: type = RC_dx11UAV; break;
        default: continue;
        }

        u16 r_index;
        if (destination & RC_dest_pixel)
            r_index = ResDesc.BindPoint + CTexture::rstPixel;
        else if (destination & RC_dest_vertex)
            r_index = ResDesc.BindPoint + CTexture::rstVertex;
        else if (destination & RC_dest_geometry)
            r_index = ResDesc.BindPoint + CTexture::rstGeometry;
        else if (destination & RC_dest_hull)
            r_index = ResDesc.BindPoint + CTexture::rstHull;
        else if (destination & RC_dest_domain)
            r_index = ResDesc.BindPoint + CTexture::rstDomain;
        else if (destination & RC_dest_compute)
            r_index = ResDesc.BindPoint + CTexture::rstCompute;
        else
            XR_PANIC("invalid constant table destination", destination);

        ref_constant C = get(ResDesc.Name);
        if (!C)
        {
            C = table.emplace_back(xr_new<R_constant>());
            C->name._set(ResDesc.Name);
            C->destination = RC_dest_sampler;
            C->type = type;
            R_constant_load& L = C->samp;
            L.index = r_index;
            L.cls = type;
        }
        else
        {
            const auto& L = C->samp;
            XR_ASSERT(C->destination == RC_dest_sampler && C->type == type, "", ResDesc.Name, C->destination, RC_dest_sampler, C->type, type);
            XR_ASSERT(L.index == r_index && L.cls == type, "", ResDesc.Name, L.index, r_index, L.cls, type);
        }
    }

    return TRUE;
}

namespace
{
[[nodiscard]] constexpr u32 dest_to_shift_value(u32 destination)
{
    switch (destination & 0xFF)
    {
    case RC_dest_vertex: return RC_dest_vertex_cb_index_shift;
    case RC_dest_pixel: return RC_dest_pixel_cb_index_shift;
    case RC_dest_geometry: return RC_dest_geometry_cb_index_shift;
    case RC_dest_hull: return RC_dest_hull_cb_index_shift;
    case RC_dest_domain: return RC_dest_domain_cb_index_shift;
    case RC_dest_compute: return RC_dest_compute_cb_index_shift;
    default: XR_PANIC("invalid shader constant destination", destination);
    }
}

[[nodiscard]] constexpr u32 dest_to_cbuf_type(u32 destination)
{
    switch (destination & 0xFF)
    {
    case RC_dest_vertex: return CB_BufferVertexShader;
    case RC_dest_pixel: return CB_BufferPixelShader;
    case RC_dest_geometry: return CB_BufferGeometryShader;
    case RC_dest_hull: return CB_BufferHullShader;
    case RC_dest_domain: return CB_BufferDomainShader;
    case RC_dest_compute: return CB_BufferComputeShader;
    default: XR_PANIC("invalid constant buffer destination", destination);
    }
}

template <class V, class K, class... Args>
constexpr auto emplace_back(V& v, K&& k, Args&&... args)
{
    return v.emplace_back(std::piecewise_construct, std::forward_as_tuple(std::forward<K>(k)), std::forward_as_tuple(std::forward<Args>(args)...));
}
} // namespace

BOOL R_constant_table::parse(void* _desc, u32 destination)
{
    auto pReflection = static_cast<ID3DShaderReflection*>(_desc);
    D3D_SHADER_DESC ShaderDesc{};
    pReflection->GetDesc(&ShaderDesc);

    if (ShaderDesc.ConstantBuffers > 0)
    {
        for (auto& tbl : m_CBTable)
            tbl.reserve(ShaderDesc.ConstantBuffers);

        for (u32 iBuf{0}; iBuf < ShaderDesc.ConstantBuffers; ++iBuf)
        {
            //	Parse single constant table
            auto pTable = pReflection->GetConstantBufferByIndex(iBuf);
            if (pTable == nullptr)
                continue;

            //	Encode buffer index into destination
            const u32 updatedDest = destination | (iBuf << dest_to_shift_value(destination));
            //	Encode bind dest (pixel/vertex buffer) and bind point index
            const u32 uiBufferIndex = iBuf | dest_to_cbuf_type(destination);

            std::ignore = parseConstants(pTable, updatedDest);

            for (auto [id, tbl] : std::views::enumerate(m_CBTable))
                emplace_back(tbl, uiBufferIndex, RImplementation.Resources->_CreateConstantBuffer(id, pTable));
        }
    }

    if (ShaderDesc.BoundResources)
        std::ignore = parseResources(pReflection, ShaderDesc.BoundResources, destination);

    std::ranges::sort(table, {}, [] [[nodiscard]] (const auto& constant) { return std::string_view{constant->name}; });

    return TRUE;
}
