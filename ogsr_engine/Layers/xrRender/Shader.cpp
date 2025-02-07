// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Shader.h"
#include "ResourceManager.h"

#include "dxRenderDeviceRender.h"

//
STextureList::~STextureList() { DEV->_DeleteTextureList(this); }

SMatrixList::~SMatrixList() { DEV->_DeleteMatrixList(this); }

SConstantList::~SConstantList() { DEV->_DeleteConstantList(this); }

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
    if (C != other.C)
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
    u32 dwTextureStage = 0;

    STextureList::const_iterator _it = this->begin();
    STextureList::const_iterator _end = this->end();
    for (; _it != _end; _it++)
    {
        const std::pair<u32, ref_texture>& loader = *_it;

        //	Shadowmap texture always uses 0 texture unit
        if (loader.second->cName == TexName)
        {
            //	Assign correct texture
            dwTextureStage = loader.first;
            break;
        }
    }

    VERIFY(_it != _end);

    return dwTextureStage;
}
