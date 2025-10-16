#include "stdafx.h"

#include "../../xr_3da/render.h"

#include "../xrRender/ResourceManager.h"
#include "../xrRender/tss.h"
#include "../xrRender/blenders/blender.h"
#include "../xrRender/blenders/blender_recorder.h"

#include "../xrRenderDX10/dx10BufferUtils.h"
#include "../xrRenderDX10/dx10ConstantBuffer.h"

#include "../xrRender/ShaderResourceTraits.h"

#include <Utilities\FlexibleVertexFormat.h>

SHS* CResourceManager::_CreateHS(LPCSTR Name) { return CreateShader<SHS>(Name); }

void CResourceManager::_DeleteHS(const SHS* HS) { DestroyShader(HS); }

SDS* CResourceManager::_CreateDS(LPCSTR Name) { return CreateShader<SDS>(Name); }

void CResourceManager::_DeleteDS(const SDS* DS) { DestroyShader(DS); }

SCS* CResourceManager::_CreateCS(LPCSTR Name) { return CreateShader<SCS>(Name); }

void CResourceManager::_DeleteCS(const SCS* CS) { DestroyShader(CS); }

//--------------------------------------------------------------------------------------------------------------
SState* CResourceManager::_CreateState(SimulatorStates& state_code)
{
    // Search equal state-code
    for (SState* C : v_states)
    {
        SimulatorStates& base = C->state_code;
        if (base.equal(state_code))
            return C;
    }

    // Create New
    SState* S = v_states.emplace_back(xr_new<SState>());
    S->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    S->state = ID3DState::Create(state_code);
    S->state_code = state_code;

    return S;
}
void CResourceManager::_DeleteState(const SState* state)
{
    if (0 == (state->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_states, state))
        return;
    Msg("! ERROR: Failed to find compiled stateblock");
}

//--------------------------------------------------------------------------------------------------------------
SPass* CResourceManager::_CreatePass(const SPass& proto)
{
    for (SPass* pass : v_passes)
        if (pass->equal(proto))
            return pass;

    SPass* P = v_passes.emplace_back(xr_new<SPass>());
    P->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    P->state = proto.state;
    P->ps = proto.ps;
    P->vs = proto.vs;
    P->gs = proto.gs;
    P->hs = proto.hs;
    P->ds = proto.ds;
    P->cs = proto.cs;
    P->constants = proto.constants;
    P->T = proto.T;

    return P;
}

void CResourceManager::_DeletePass(const SPass* P)
{
    if (0 == (P->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_passes, P))
        return;
    Msg("! ERROR: Failed to find compiled pass");
}

//--------------------------------------------------------------------------------------------------------------
SVS* CResourceManager::_CreateVS(LPCSTR _name)
{
    int skinning = -1;
    string_path name;
    xr_strcpy(name, _name);
    if (0 == RImplementation.m_skinning)
    {
        xr_strcat(name, "_0");
        skinning = 0;
    }
    if (1 == RImplementation.m_skinning)
    {
        xr_strcat(name, "_1");
        skinning = 1;
    }
    if (2 == RImplementation.m_skinning)
    {
        xr_strcat(name, "_2");
        skinning = 2;
    }
    if (3 == RImplementation.m_skinning)
    {
        xr_strcat(name, "_3");
        skinning = 3;
    }
    if (4 == RImplementation.m_skinning)
    {
        xr_strcat(name, "_4");
        skinning = 4;
    }
    LPSTR N = LPSTR(name);
    map_VS::iterator I = m_vs.find(N);
    if (I != m_vs.end())
        return I->second;
    else
    {
        SVS* _vs = xr_new<SVS>();
        _vs->skinning = skinning;
        _vs->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        m_vs.try_emplace(_vs->set_name(name), _vs);

        if (!_stricmp(_name, "null"))
            return _vs;

        string_path shName;
        {
            const char* pchr = strchr(_name, '(');
            ptrdiff_t size = pchr ? pchr - _name : xr_strlen(_name);
            strncpy_s(shName, _name, size);
            shName[size] = 0;
        }

        string_path cname;
        strconcat(sizeof(cname), cname, RImplementation.getShaderPath(), /*_name*/ shName, ".vs");
        FS.update_path(cname, "$game_shaders$", cname);

        IReader* file = FS.r_open(cname);
        R_ASSERT2(file, cname);

        file->skip_bom(cname);

        const std::string_view strbuf{reinterpret_cast<const char*>(file->pointer()), static_cast<size_t>(file->elapsed())};

        // Select target
        LPCSTR c_target = "vs_5_0";
        LPCSTR c_entry = "main";

        DWORD Flags{D3DCOMPILE_FLAGS_DEFAULT};
        if (strstr(Core.Params, "-shadersdbg"))
            Flags |= D3DCOMPILE_FLAGS_DEBUG;

        HRESULT const _hr =
            RImplementation.shader_compile(name, reinterpret_cast<DWORD const*>(strbuf.data()), static_cast<UINT>(strbuf.size()), c_entry, c_target, Flags, (void*&)_vs);

        FS.r_close(file);

        ASSERT_FMT(!FAILED(_hr), "Can't compile shader [%s]", name);

        return _vs;
    }
}

void CResourceManager::_DeleteVS(const SVS* vs)
{
    if (0 == (vs->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    auto I = m_vs.find(*vs->cName);
    if (I != m_vs.end())
    {
        m_vs.erase(I);
        xr_vector<SDeclaration*>::iterator iDecl;
        for (iDecl = v_declarations.begin(); iDecl != v_declarations.end(); ++iDecl)
        {
            xr_map<ID3DBlob*, ID3DInputLayout*>::iterator iLayout;
            iLayout = (*iDecl)->vs_to_layout.find(vs->signature->signature);
            if (iLayout != (*iDecl)->vs_to_layout.end())
            {
                //	Release vertex layout
                _RELEASE(iLayout->second);
                (*iDecl)->vs_to_layout.erase(iLayout);
            }
        }

        return;
    }

    Msg("! ERROR: Failed to find compiled vertex-shader '%s'", *vs->cName);
}

//--------------------------------------------------------------------------------------------------------------
SPS* CResourceManager::_CreatePS(LPCSTR _name)
{
    string_path name;
    xr_strcpy(name, _name);
    if (0 == RImplementation.m_MSAASample)
        xr_strcat(name, "_0");
    if (1 == RImplementation.m_MSAASample)
        xr_strcat(name, "_1");
    if (2 == RImplementation.m_MSAASample)
        xr_strcat(name, "_2");
    if (3 == RImplementation.m_MSAASample)
        xr_strcat(name, "_3");
    if (4 == RImplementation.m_MSAASample)
        xr_strcat(name, "_4");
    if (5 == RImplementation.m_MSAASample)
        xr_strcat(name, "_5");
    if (6 == RImplementation.m_MSAASample)
        xr_strcat(name, "_6");
    if (7 == RImplementation.m_MSAASample)
        xr_strcat(name, "_7");
    LPSTR N = LPSTR(name);
    map_PS::iterator I = m_ps.find(N);
    if (I != m_ps.end())
        return I->second;
    else
    {
        SPS* _ps = xr_new<SPS>();
        _ps->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        m_ps.try_emplace(_ps->set_name(name), _ps);

        if (!_stricmp(_name, "null"))
        {
            _ps->ps = nullptr;
            return _ps;
        }

        string_path shName;
        const char* pchr = strchr(_name, '(');
        ptrdiff_t strSize = pchr ? pchr - _name : xr_strlen(_name);
        strncpy_s(shName, _name, strSize);
        shName[strSize] = 0;

        // Open file
        string_path cname;
        strconcat(sizeof(cname), cname, RImplementation.getShaderPath(), /*_name*/ shName, ".ps");
        FS.update_path(cname, "$game_shaders$", cname);

        IReader* file = FS.r_open(cname);
        R_ASSERT2(file, cname);

        file->skip_bom(cname);

        const std::string_view strbuf{reinterpret_cast<const char*>(file->pointer()), static_cast<size_t>(file->elapsed())};

        // Select target
        LPCSTR c_target = "ps_5_0";
        LPCSTR c_entry = "main";

        DWORD Flags{D3DCOMPILE_FLAGS_DEFAULT};
        if (strstr(Core.Params, "-shadersdbg"))
            Flags |= D3DCOMPILE_FLAGS_DEBUG;

        HRESULT const _hr =
            RImplementation.shader_compile(name, reinterpret_cast<DWORD const*>(strbuf.data()), static_cast<UINT>(strbuf.size()), c_entry, c_target, Flags, (void*&)_ps);

        FS.r_close(file);

        ASSERT_FMT(!FAILED(_hr), "Can't compile shader [%s]", name);

        return _ps;
    }
}

void CResourceManager::_DeletePS(const SPS* ps)
{
    if (0 == (ps->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    auto I = m_ps.find(*ps->cName);
    if (I != m_ps.end())
    {
        m_ps.erase(I);
        return;
    }

    Msg("! ERROR: Failed to find compiled pixel-shader '%s'", *ps->cName);
}

//--------------------------------------------------------------------------------------------------------------
SGS* CResourceManager::_CreateGS(LPCSTR name)
{
    auto I = m_gs.find(name);
    if (I != m_gs.end())
    {
        return I->second;
    }
    else
    {
        SGS* _gs = xr_new<SGS>();
        _gs->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        m_gs.try_emplace(_gs->set_name(name), _gs);

        if (!_stricmp(name, "null"))
        {
            _gs->gs = nullptr;
            return _gs;
        }

        // Open file
        string_path cname;
        strconcat(sizeof(cname), cname, RImplementation.getShaderPath(), name, ".gs");
        FS.update_path(cname, "$game_shaders$", cname);

        IReader* file = FS.r_open(cname);
        R_ASSERT2(file, cname);

        file->skip_bom(cname);

        // Select target
        LPCSTR c_target = "gs_5_0";
        LPCSTR c_entry = "main";

        DWORD Flags{D3DCOMPILE_FLAGS_DEFAULT};
        if (strstr(Core.Params, "-shadersdbg"))
            Flags |= D3DCOMPILE_FLAGS_DEBUG;

        HRESULT const _hr = RImplementation.shader_compile(name, (DWORD const*)file->pointer(), file->elapsed(), c_entry, c_target, Flags, (void*&)_gs);

        FS.r_close(file);

        ASSERT_FMT(!FAILED(_hr), "Can't compile shader [%s]", name);

        return _gs;
    }
}

void CResourceManager::_DeleteGS(const SGS* gs)
{
    if (0 == (gs->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    auto I = m_gs.find(*gs->cName);
    if (I != m_gs.end())
    {
        m_gs.erase(I);
        return;
    }

    Msg("! ERROR: Failed to find compiled geometry shader '%s'", *gs->cName);
}

//--------------------------------------------------------------------------------------------------------------
static BOOL dcl_equal(const D3DVERTEXELEMENT9* a, const D3DVERTEXELEMENT9* b)
{
    // check sizes
    u32 a_size = FVF::GetDeclLength(a);
    u32 b_size = FVF::GetDeclLength(b);
    if (a_size != b_size)
        return FALSE;
    return 0 == memcmp(a, b, a_size * sizeof(D3DVERTEXELEMENT9));
}

SDeclaration* CResourceManager::_CreateDecl(const D3DVERTEXELEMENT9* dcl)
{
    // Search equal code
    for (SDeclaration* D : v_declarations)
    {
        if (dcl_equal(dcl, &D->dcl_code.front()))
            return D;
    }

    // Create _new
    SDeclaration* D = v_declarations.emplace_back(xr_new<SDeclaration>());
    u32 dcl_size = FVF::GetDeclLength(dcl) + 1;
    D->dcl_code.assign(dcl, dcl + dcl_size);
    dx10BufferUtils::ConvertVertexDeclaration(D->dcl_code, D->dx10_dcl_code);
    D->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    return D;
}

void CResourceManager::_DeleteDecl(const SDeclaration* dcl)
{
    if (0 == (dcl->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_declarations, dcl))
        return;
    Msg("! ERROR: Failed to find compiled vertex-declarator");
}

//--------------------------------------------------------------------------------------------------------------
R_constant_table* CResourceManager::_CreateConstantTable(const R_constant_table& C)
{
    if (C.empty())
        return nullptr;

    for (R_constant_table* table : v_constant_tables)
        if (table->equal(C))
            return table;

    R_constant_table* table = v_constant_tables.emplace_back(xr_new<R_constant_table>());
    table->clone(C);
    table->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    return table;
}

void CResourceManager::_DeleteConstantTable(const R_constant_table* C)
{
    if (0 == (C->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_constant_tables, C))
        return;
    Msg("! ERROR: Failed to find compiled constant-table");
}

//--------------------------------------------------------------------------------------------------------------
CRT* CResourceManager::_CreateRT(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 sampleCount /* = 1 */, u32 slices_num /*=1*/, Flags32 flags /*= {}*/)
{
    R_ASSERT(Name && Name[0] && w && h);

    // ***** first pass - search already created RT
    auto I = m_rtargets.find(Name);
    if (I != m_rtargets.end())
    {
        return I->second;
    }
    else
    {
        CRT* RT = xr_new<CRT>();
        RT->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        m_rtargets.emplace(RT->set_name(Name), RT);

        if (Device.b_is_Ready)
            RT->create(Name, w, h, f, sampleCount, slices_num, flags);

        return RT;
    }
}

void CResourceManager::_DeleteRT(const CRT* RT)
{
    if (0 == (RT->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    auto I = m_rtargets.find(*RT->cName);
    if (I != m_rtargets.end())
    {
        m_rtargets.erase(I);
        return;
    }

    Msg("! ERROR: Failed to find render-target '%s'", *RT->cName);
}

//--------------------------------------------------------------------------------------------------------------

SGeometry* CResourceManager::CreateGeom(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib)
{
    R_ASSERT(decl && vb);

    SDeclaration* dcl = _CreateDecl(decl);
    u32 vb_stride = FVF::ComputeVertexSize(decl, 0);

    // ***** first pass - search already loaded shader
    for (SGeometry* geom : v_geoms)
    {
        SGeometry& G = *geom;
        if (G.dcl == dcl && G.vb == vb && G.ib == ib && G.vb_stride == vb_stride)
            return geom;
    }

    SGeometry* Geom = v_geoms.emplace_back(xr_new<SGeometry>());
    Geom->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    Geom->dcl = dcl;
    Geom->vb = vb;
    Geom->vb_stride = vb_stride;
    Geom->ib = ib;

    return Geom;
}

SGeometry* CResourceManager::CreateGeom(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib)
{
    auto dcl = std::vector<D3DVERTEXELEMENT9>(MAXD3DDECLLENGTH + 1);
    CHK_DX(FVF::CreateDeclFromFVF(FVF, dcl));
    SGeometry* g = CreateGeom(dcl.data(), vb, ib);
    return g;
}

void CResourceManager::DeleteGeom(const SGeometry* Geom)
{
    if (0 == (Geom->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_geoms, Geom))
        return;
    Msg("! ERROR: Failed to find compiled geometry-declaration");
}

CTexture* CResourceManager::_CreateTexture(LPCSTR _Name)
{
    if (0 == xr_strcmp(_Name, "null"))
        return nullptr;

    R_ASSERT(_Name && _Name[0]);
    string_path Name;
    xr_strcpy(Name, _Name); //. andy if (strext(Name)) *strext(Name)=0;
    fix_texture_name(Name);
    // ***** first pass - search already loaded texture
    LPSTR N = LPSTR(Name);
    map_TextureIt I = m_textures.find(N);
    if (I != m_textures.end())
        return I->second;

    CTexture* T = xr_new<CTexture>();
    T->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    m_textures.emplace(T->set_name(Name), T);
    T->Preload();
    if (Device.b_is_Ready && !bDeferredLoad)
        T->Load();
    return T;
}

void CResourceManager::_DeleteTexture(const CTexture* T)
{
    if (0 == (T->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    auto I = m_textures.find(*T->cName);
    if (I != m_textures.end())
    {
        m_textures.erase(I);
        return;
    }

    Msg("! ERROR: Failed to find texture surface '%s'", *T->cName);
}

#ifdef DEBUG
void CResourceManager::DBG_VerifyTextures()
{
    map_Texture::iterator I = m_textures.begin();
    map_Texture::iterator E = m_textures.end();
    for (; I != E; I++)
    {
        R_ASSERT(I->first);
        R_ASSERT(I->second);
        R_ASSERT(I->second->cName);
        R_ASSERT(0 == xr_strcmp(I->first, *I->second->cName));
    }
}
#endif

STextureList* CResourceManager::_CreateTextureList(STextureList& L)
{
    std::ranges::sort(L, [](const std::pair<u32, ref_texture>& _1, const std::pair<u32, ref_texture>& _2) { return _1.first < _2.first; });

    for (STextureList* base : lst_textures)
    {
        if (L.equal(*base))
            return base;
    }

    STextureList* lst = lst_textures.emplace_back(xr_new<STextureList>());
    lst->clone(L);
    lst->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    return lst;
}

void CResourceManager::_DeleteTextureList(const STextureList* L)
{
    if (0 == (L->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(lst_textures, L))
        return;
    Msg("! ERROR: Failed to find compiled list of textures");
}

dx10ConstantBuffer* CResourceManager::_CreateConstantBuffer(ctx_id_t context_id, ID3DShaderReflectionConstantBuffer* pTable)
{
    VERIFY(pTable);
    dx10ConstantBuffer* pTempBuffer = xr_new<dx10ConstantBuffer>(pTable);

    for (dx10ConstantBuffer* buf : v_constant_buffer[context_id])
    {
        if (pTempBuffer->Similar(*buf))
        {
            xr_delete(pTempBuffer);
            return buf;
        }
    }

    pTempBuffer->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_constant_buffer[context_id].emplace_back(pTempBuffer);
    return pTempBuffer;
}

void CResourceManager::_DeleteConstantBuffer(const dx10ConstantBuffer* pBuffer)
{
    if (!(pBuffer->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
        if (reclaim(v_constant_buffer[id], pBuffer))
            return;

    Msg("! ERROR: Failed to find compiled constant buffer");
}

//--------------------------------------------------------------------------------------------------------------
SInputSignature* CResourceManager::_CreateInputSignature(ID3DBlob* pBlob)
{
    VERIFY(pBlob);

    for (SInputSignature* sign : v_input_signature)
    {
        if ((pBlob->GetBufferSize() == sign->signature->GetBufferSize()) && (!(memcmp(pBlob->GetBufferPointer(), sign->signature->GetBufferPointer(), pBlob->GetBufferSize()))))
        {
            return sign;
        }
    }

    SInputSignature* pSign = v_input_signature.emplace_back(xr_new<SInputSignature>(pBlob));
    pSign->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    return pSign;
}

void CResourceManager::_DeleteInputSignature(const SInputSignature* pSignature)
{
    if (0 == (pSignature->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_input_signature, pSignature))
        return;
    Msg("! ERROR: Failed to find input signature");
}
