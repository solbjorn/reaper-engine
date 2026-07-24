// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Shader.h"

#include "ResourceManager.h"

STextureList::~STextureList() { RImplementation.Resources->_DeleteTextureList(this); }
SPass::~SPass() { RImplementation.Resources->_DeletePass(this); }
ShaderElement::~ShaderElement() { RImplementation.Resources->_DeleteElement(this); }
SGeometry::~SGeometry() { RImplementation.Resources->DeleteGeom(this); }
Shader::~Shader() { RImplementation.Resources->Delete(this); }

//////////////////////////////////////////////////////////////////////////

void resptrcode_shader::create(const char* s_shader, const char* s_textures) { _set(RImplementation.Resources->Create(s_shader, s_textures)); }
void resptrcode_shader::create(IBlender* B, const char* s_shader, const char* s_textures) { _set(RImplementation.Resources->Create(B, s_shader, s_textures)); }

//////////////////////////////////////////////////////////////////////////

void resptrcode_geom::create(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib) { _set(RImplementation.Resources->CreateGeom(FVF, vb, ib)); }

void resptrcode_geom::create(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib)
{
    _set(RImplementation.Resources->CreateGeom(decl, vb, ib));
}

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
    flags.aref = false;

    // SSS Custom rendering
    flags.isLandscape = FALSE;
    flags.isWater = FALSE;
}

bool ShaderElement::equal(const ShaderElement& S) const
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
    if (flags.aref != S.flags.aref)
        return false;

    return passes == S.passes;
}

bool Shader::equal(const Shader* S, int index) const
{
    if (nullptr == E[index] && nullptr == S->E[index])
        return TRUE;
    if (nullptr == E[index] || nullptr == S->E[index])
        return FALSE;

    return E[index]->equal(*S->E[index]);
}

bool Shader::equal(const Shader* S) const
{
    for (int i{0}; i < SHADER_ELEMENTS_MAX; i++)
    {
        if (!equal(S, i))
            return false;
    }

    return true;
}

void STextureList::clear()
{
    for (auto& it : *this)
        it.second.destroy();

    inherited_vec::clear();
}

u32 STextureList::find_texture_stage(const shared_str& TexName) const
{
    return XR_ASSERT_VAL(std::ranges::find_if(*this, [&TexName] [[nodiscard]] (const auto& elem) { return elem.second && elem.second->cName == TexName; }) !=
                             this->end(),
                         "texture stage not found", TexName)
        ->first;
}

void STextureList::create_texture(u32 stage, const char* textureName)
{
    if (const auto it = std::ranges::find_if(*this, [stage] [[nodiscard]] (const auto& elem) { return elem.first == stage && !elem.second; });
        it != this->end())
        it->second.create(textureName);
}
