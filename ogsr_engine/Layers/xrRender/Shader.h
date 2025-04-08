// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

#include "r_constants.h"
#include "../../xrCore/xr_resource.h"

#include "sh_atomic.h"
#include "sh_texture.h"
#include "sh_rt.h"

using sh_list = xr_vector<shared_str>;

class CBlender_Compile;
class IBlender;
class IBlenderXr;

#define SHADER_PASSES_MAX 2
#define SHADER_ELEMENTS_MAX 16

//////////////////////////////////////////////////////////////////////////
struct STextureList : public xr_resource_flagged, public xr_vector<std::pair<u32, ref_texture>>
{
    using inherited_vec = xr_vector<std::pair<u32, ref_texture>>;
    STextureList() = default;
    ~STextureList();

    IC BOOL equal(const STextureList& base) const
    {
        if (size() != base.size())
            return FALSE;
        for (u32 cmp = 0; cmp < size(); cmp++)
        {
            if ((*this)[cmp].first != base[cmp].first)
                return FALSE;
            if ((*this)[cmp].second != base[cmp].second)
                return FALSE;
        }
        return TRUE;
    }
    void clear();

    // Avoid using these functions.
    // If possible use precompiled texture list.
    u32 find_texture_stage(const shared_str& TexName) const;
    void create_texture(u32 stage, const char* textureName);
};
typedef resptr_core<STextureList, resptr_base<STextureList>> ref_texture_list;

//////////////////////////////////////////////////////////////////////////
struct SGeometry : public xr_resource_flagged
{
    ref_declaration dcl;
    ID3DVertexBuffer* vb;
    ID3DIndexBuffer* ib;
    u32 vb_stride;
    SGeometry() = default;
    ~SGeometry();
};

struct resptrcode_geom : public resptr_base<SGeometry>
{
    void create(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
    void create(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
    void destroy() { _set(nullptr); }
    u32 stride() const { return _get()->vb_stride; }
};

typedef resptr_core<SGeometry, resptrcode_geom> ref_geom;

//////////////////////////////////////////////////////////////////////////
struct SPass : public xr_resource_flagged
{
    ref_state state; // Generic state, like Z-Buffering, samplers, etc
    ref_ps ps; // may be NULL = FFP, in that case "state" must contain TSS setup
    ref_vs vs; // may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used
    ref_gs gs; // may be NULL = don't use geometry shader at all
    ref_hs hs; // may be NULL = don't use hull shader at all
    ref_ds ds; // may be NULL = don't use domain shader at all
    ref_cs cs; // may be NULL = don't use compute shader at all

    ref_ctable constants; // may be NULL
    ref_texture_list T;

    SPass() = default;
    ~SPass();

    bool equal(const SPass& other) const;
};
typedef resptr_core<SPass, resptr_base<SPass>> ref_pass;

//////////////////////////////////////////////////////////////////////////
struct ShaderElement : public xr_resource_flagged
{
    struct Sflags
    {
        u32 iPriority : 2;
        u32 bStrictB2F : 1;
        u32 bEmissive : 1;
        u32 bDistort : 1;
        u32 bWmark : 1;
        u32 bLandscape : 1;
        u32 isLandscape : 1;
        u32 isWater : 1;
    };

    Sflags flags;
    svector<ref_pass, SHADER_PASSES_MAX> passes;

    ShaderElement();
    ~ShaderElement();
    BOOL equal(ShaderElement& S);
};

using ref_selement = resptr_core<ShaderElement, resptr_base<ShaderElement>>;

//////////////////////////////////////////////////////////////////////////
struct Shader : public xr_resource_flagged
{
    ref_selement E[6]; // R1 - 0=norm_lod0(det),	1=norm_lod1(normal),	2=L_point,		3=L_spot,	4=L_for_models,
                       // R2 - 0=deffer,			1=norm_lod1(normal),	2=psm,			3=ssm,		4=dsm
    Shader() = default;
    ~Shader();
    BOOL equal(Shader* S);
    BOOL equal(Shader* S, int index);
};

struct resptrcode_shader : public resptr_base<Shader>
{
    void create(const char* s_shader, const char* s_textures = nullptr);
    void create(IBlender* B, const char* s_shader, const char* s_textures = nullptr);

    template <class Blender, bool msaa = false>
    void create(const char* s_shader, const char* s_textures = nullptr)
    {
        Blender blender;

        if constexpr (msaa)
            blender.SetDefine("ISAMPLE", "0");

        create(&blender, s_shader, s_textures);
    }

    void destroy() { _set(NULL); }
};

using ref_shader = resptr_core<Shader, resptrcode_shader>;

enum SE_R1
{
    SE_R1_NORMAL_HQ = 0, // high quality/detail
    SE_R1_NORMAL_LQ = 1, // normal or low quality
    SE_R1_LPOINT = 2, // add: point light
    SE_R1_LSPOT = 3, // add:	spot light
    SE_R1_LMODELS = 4, // lighting info for models or shadowing from models
};

//	E[3] - can use for night vision but need to extend SE_R1. Will need
//	Extra shader element.
//	E[4] - distortion or self illumination(self emission).
//	E[4] Can use for lightmap capturing.

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
