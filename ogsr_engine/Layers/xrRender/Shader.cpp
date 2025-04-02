// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Shader.h"
#include "ResourceManager.h"

#include "dxRenderDeviceRender.h"

//
STextureList::~STextureList() { DEV->_DeleteTextureList(this); }

SPass::~SPass() { DEV->_DeletePass(this); }

ShaderElement::~ShaderElement() { DEV->_DeleteElement(this); }

SGeometry::~SGeometry() { DEV->DeleteGeom(this); }

Shader::~Shader() { DEV->Delete(this); }

//////////////////////////////////////////////////////////////////////////
void resptrcode_shader::create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices) { _set(DEV->Create(s_shader, s_textures, s_constants, s_matrices)); }
void resptrcode_shader::create(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
    _set(DEV->Create(B, s_shader, s_textures, s_constants, s_matrices));
}

//////////////////////////////////////////////////////////////////////////
void resptrcode_geom::create(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib) { _set(DEV->CreateGeom(FVF, vb, ib)); }
void resptrcode_geom::create(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib) { _set(DEV->CreateGeom(decl, vb, ib)); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool SPass::equal(const SPass& other) const
{
    if (state != other.state)
        return false;
    if (ps != other.ps)
        return false;
    if (vs != other.vs)
        return false;
    if (gs != other.gs)
        return false;
    if (hs != other.hs)
        return false;
    if (ds != other.ds)
        return false;
    if (cs != other.cs)
        return false;
    if (constants != other.constants)
        return false; // is this nessesary??? (ps+vs already combines)
    if (T != other.T)
        return false;

    return true;
}

//
ShaderElement::ShaderElement()
{
    flags.iPriority = 1;
    flags.bStrictB2F = FALSE;
    flags.bEmissive = FALSE;
    flags.bDistort = FALSE;
    flags.bWmark = FALSE;
    flags.bLandscape = FALSE;

    // SSS Custom rendering
    flags.isLandscape = FALSE;
    flags.isWater = FALSE;
}

BOOL ShaderElement::equal(ShaderElement& S)
{
    if (flags.iPriority != S.flags.iPriority)
        return FALSE;
    if (flags.bStrictB2F != S.flags.bStrictB2F)
        return FALSE;
    if (flags.bEmissive != S.flags.bEmissive)
        return FALSE;
    if (flags.bLandscape != S.flags.bLandscape)
        return FALSE;
    if (flags.bWmark != S.flags.bWmark)
        return FALSE;
    if (flags.bDistort != S.flags.bDistort)
        return FALSE;
    if (passes.size() != S.passes.size())
        return FALSE;
    for (u32 p = 0; p < passes.size(); p++)
        if (passes[p] != S.passes[p])
            return FALSE;
    return TRUE;
}

BOOL Shader::equal(Shader* S, int index)
{
    if (nullptr == E[index] && nullptr == S->E[index])
        return TRUE;
    if (nullptr == E[index] || nullptr == S->E[index])
        return FALSE;

    return E[index]->equal(*S->E[index]);
}

BOOL Shader::equal(Shader* S)
{
    for (int i = 0; i < 5; i++)
    {
        if (!equal(S, i))
            return FALSE;
    }
    return TRUE;
}

void STextureList::clear()
{
    for (auto& it : *this)
        it.second.destroy();

    __super::clear();
}

u32 STextureList::find_texture_stage(const shared_str& TexName) const
{
    for (const auto& [stage, texture] : *this)
    {
        if (texture && texture->cName == TexName)
            return stage;
    }

    VERIFY3(false, "Couldn't find texture stage", TexName.c_str());

    return 0;
}

void STextureList::create_texture(u32 stage, const char* textureName)
{
    for (auto& [id, texture] : *this)
    {
        if (id == stage && !texture)
        {
            // Assign correct texture
            texture.create(textureName);
        }
    }
}
