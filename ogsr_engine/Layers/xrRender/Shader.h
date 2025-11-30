// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SHADER__INCLUDED_
#define SHADER__INCLUDED_

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
constexpr inline gsl::index SHADER_ELEMENTS_MAX{6};

//////////////////////////////////////////////////////////////////////////

struct STextureList : public xr_resource_flagged, public xr_vector<std::pair<u32, ref_texture>>
{
    RTTI_DECLARE_TYPEINFO(STextureList, xr_resource_flagged);

public:
    using inherited_vec = xr_vector<std::pair<u32, ref_texture>>;

    STextureList() = default;
    ~STextureList() override;

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

    void clone(const STextureList& from)
    {
        xr_resource_flagged::clone(from);
        *static_cast<inherited_vec*>(this) = from;
    }
};
typedef resptr_core<STextureList, resptr_base<STextureList>> ref_texture_list;

//////////////////////////////////////////////////////////////////////////

struct SGeometry : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(SGeometry, xr_resource_flagged);

public:
    ref_declaration dcl;
    ID3DVertexBuffer* vb;
    ID3DIndexBuffer* ib;
    u32 vb_stride;

    SGeometry() = default;
    ~SGeometry() override;
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
    RTTI_DECLARE_TYPEINFO(SPass, xr_resource_flagged);

public:
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
    ~SPass() override;

    bool equal(const SPass& other) const;
};
typedef resptr_core<SPass, resptr_base<SPass>> ref_pass;

//////////////////////////////////////////////////////////////////////////

struct ShaderElement : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(ShaderElement, xr_resource_flagged);

public:
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
    ~ShaderElement() override;

    bool equal(const ShaderElement& S) const;

    void clone(ShaderElement&& from)
    {
        xr_resource_flagged::clone(from);

        flags = std::move(from.flags);
        passes = std::move(from.passes);
    }
};

using ref_selement = resptr_core<ShaderElement, resptr_base<ShaderElement>>;

//////////////////////////////////////////////////////////////////////////

struct Shader : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(Shader, xr_resource_flagged);

public:
    std::array<ref_selement, SHADER_ELEMENTS_MAX> E;

    Shader() = default;
    ~Shader() override;

    bool equal(const Shader* S) const;
    bool equal(const Shader* S, int index) const;

    void clone(Shader&& from)
    {
        xr_resource_flagged::clone(from);
        E = std::move(from.E);
    }
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

    void destroy() { _set(nullptr); }
};

using ref_shader = resptr_core<Shader, resptrcode_shader>;

#endif // SHADER__INCLUDED_
