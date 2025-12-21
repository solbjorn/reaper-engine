#include "stdafx.h"

#include "../xrRender/ResourceManager.h"

#include "../xrRender/tss.h"
#include "../xrRender/blenders/blender.h"
#include "../xrRender/blenders/blender_recorder.h"
#include "../xrRender/dxRenderDeviceRender.h"

#include "../../xrScriptEngine/xr_sol.h"
#include "../../xr_3da/Render.h"

#include <format>

namespace
{
// wrapper
class adopt_dx10sampler
{
    CBlender_Compile* m_pC;
    u32 m_SI; //	Sampler index

public:
    explicit adopt_dx10sampler(CBlender_Compile* C, u32 SamplerIndex) : m_pC{C}, m_SI{SamplerIndex}
    {
        if (m_SI == std::numeric_limits<u32>::max())
            m_pC = nullptr;
    }

    adopt_dx10sampler(const adopt_dx10sampler& _C) : m_pC{_C.m_pC}, m_SI{_C.m_SI}
    {
        if (m_SI == std::numeric_limits<u32>::max())
            m_pC = nullptr;
    }

    //	adopt_sampler&			_texture		(LPCSTR texture)		{ if (C) C->i_Texture	(stage,texture);											return *this;	}
    //	adopt_sampler&			_projective		(bool _b)				{ if (C) C->i_Projective(stage,_b);													return *this;	}
    adopt_dx10sampler& _clamp()
    {
        if (m_pC)
            m_pC->i_dx10Address(m_SI, D3DTADDRESS_CLAMP);
        return *this;
    }
    //	adopt_sampler&			_wrap			()						{ if (C) C->i_Address	(stage,D3DTADDRESS_WRAP);									return *this;	}
    //	adopt_sampler&			_mirror			()						{ if (C) C->i_Address	(stage,D3DTADDRESS_MIRROR);									return *this;	}
    //	adopt_sampler&			_f_anisotropic	()						{ if (C) C->i_Filter	(stage,D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,D3DTEXF_ANISOTROPIC);	return *this;	}
    //	adopt_sampler&			_f_trilinear	()						{ if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_LINEAR);		return *this;	}
    //	adopt_sampler&			_f_bilinear		()						{ if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_POINT, D3DTEXF_LINEAR);		return *this;	}
    //	adopt_sampler&			_f_linear		()						{ if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_NONE,  D3DTEXF_LINEAR);		return *this;	}
    //	adopt_sampler&			_f_none			()						{ if (C) C->i_Filter	(stage,D3DTEXF_POINT, D3DTEXF_NONE,  D3DTEXF_POINT);		return *this;	}
    //	adopt_sampler&			_fmin_none		()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_NONE);										return *this;	}
    //	adopt_sampler&			_fmin_point		()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_POINT);										return *this;	}
    //	adopt_sampler&			_fmin_linear	()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_LINEAR);										return *this;	}
    //	adopt_sampler&			_fmin_aniso		()						{ if (C) C->i_Filter_Min(stage,D3DTEXF_ANISOTROPIC);								return *this;	}
    //	adopt_sampler&			_fmip_none		()						{ if (C) C->i_Filter_Mip(stage,D3DTEXF_NONE);										return *this;	}
    //	adopt_sampler&			_fmip_point		()						{ if (C) C->i_Filter_Mip(stage,D3DTEXF_POINT);										return *this;	}
    //	adopt_sampler&			_fmip_linear	()						{ if (C) C->i_Filter_Mip(stage,D3DTEXF_LINEAR);										return *this;	}
    //	adopt_sampler&			_fmag_none		()						{ if (C) C->i_Filter_Mag(stage,D3DTEXF_NONE);										return *this;	}
    //	adopt_sampler&			_fmag_point		()						{ if (C) C->i_Filter_Mag(stage,D3DTEXF_POINT);										return *this;	}
    //	adopt_sampler&			_fmag_linear	()						{ if (C) C->i_Filter_Mag(stage,D3DTEXF_LINEAR);										return *this;	}
};
/*
class	adopt_dx10texture
{
    CBlender_Compile*		m_pC;
    u32						m_TI;	//	Sampler index
public:
    adopt_dx10texture	(CBlender_Compile*	C, u32 TextureIndex)	: m_pC(C), m_TI(TextureIndex)		{ if (u32(-1)==m_TI) m_pC=0;}
    adopt_dx10texture	(const adopt_dx10texture&	_C)				: m_pC(_C.m_pC), m_TI(_C.m_TI)	{ if (u32(-1)==m_TI) m_pC=0;}
};
*/

// wrapper
class adopt_compiler
{
    CBlender_Compile* C;
    bool& m_bFirstPass;

    void TryEndPass() const
    {
        if (!m_bFirstPass)
            C->r_End();
        m_bFirstPass = false;
    }

public:
    explicit adopt_compiler(CBlender_Compile* _C, bool& bFirstPass) : C{_C}, m_bFirstPass{bFirstPass} { m_bFirstPass = true; }
    adopt_compiler(const adopt_compiler& _C) : C{_C.C}, m_bFirstPass{_C.m_bFirstPass} {}

    adopt_compiler& _options(int P, bool S)
    {
        C->SetParams(P, S);
        return *this;
    }
    adopt_compiler& _o_emissive(bool E)
    {
        C->SH->flags.bEmissive = E;
        return *this;
    }
    adopt_compiler& _o_distort(bool E)
    {
        C->SH->flags.bDistort = E;
        return *this;
    }
    adopt_compiler& _o_wmark(bool E)
    {
        C->SH->flags.bWmark = E;
        return *this;
    }
    adopt_compiler& _pass(LPCSTR vs, LPCSTR ps)
    {
        TryEndPass();
        C->r_Pass(vs, ps, true);
        return *this;
    }
    adopt_compiler& _passgs(LPCSTR vs, LPCSTR gs, LPCSTR ps)
    {
        TryEndPass();
        C->r_Pass(vs, gs, ps, true);
        return *this;
    }
    adopt_compiler& _fog(bool _fog)
    {
        C->PassSET_LightFog(FALSE, _fog);
        return *this;
    }
    adopt_compiler& _ZB(bool _test, bool _write)
    {
        C->PassSET_ZB(_test, _write);
        return *this;
    }
    adopt_compiler& _blend(bool _blend, u32 abSRC, u32 abDST)
    {
        C->PassSET_ablend_mode(_blend, abSRC, abDST);
        return *this;
    }
    adopt_compiler& _aref(bool _aref, u32 aref)
    {
        C->PassSET_ablend_aref(_aref, aref);
        return *this;
    }
    adopt_compiler& _dx10texture(LPCSTR _resname, LPCSTR _texname)
    {
        C->r_dx10Texture(_resname, _texname);
        return *this;
    }
    adopt_dx10sampler _dx10sampler(LPCSTR _name)
    {
        u32 s = C->r_dx10Sampler(_name);
        return adopt_dx10sampler(C, s);
    }

    //	DX10 specific
    adopt_compiler& _dx10color_write_enable(bool cR, bool cG, bool cB, bool cA)
    {
        C->r_ColorWriteEnable(cR, cG, cB, cA);
        return *this;
    }
    adopt_compiler& _dx10Stencil(bool Enable, u32 Func, u32 Mask, u32 WriteMask, u32 Fail, u32 Pass, u32 ZFail)
    {
        C->r_Stencil(Enable, Func, Mask, WriteMask, Fail, Pass, ZFail);
        return *this;
    }
    adopt_compiler& _dx10StencilRef(u32 Ref)
    {
        C->r_StencilRef(Ref);
        return *this;
    }
    adopt_compiler& _dx10CullMode(u32 Ref)
    {
        C->r_CullMode((D3DCULL)Ref);
        return *this;
    }
    adopt_compiler& _dx10ZFunc(u32 Func)
    {
        C->RS.SetRS(D3DRS_ZFUNC, Func);
        return *this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr std::string_view GlobalNamespace{"_G"};
constexpr std::string_view FILE_HEADER{
    "\
local function script_name() \
return '{0}' \
end; \
local this; \
module('{0}', package.seeall, function(m) this = m end); \
{1}"};

std::optional<sol::state> __declspec(align(std::hardware_destructive_interference_size)) lua;

void lua_panic(s32 code)
{
    xr_string data;
    size_t len;

    gsl::czstring desc = lua_tolstring(lua->lua_state(), -1, &len);
    if (desc != nullptr)
    {
        data.assign(desc, len);
        lua_pop(lua->lua_state(), 1);

        desc = data.c_str();
    }
    else
    {
        desc = "An unexpected error occurred and panic has been invoked";
    }

    xr_string expr = "script " + sol::to_string(sol::call_status{code}) + " error";
    Debug.backend(expr.c_str(), desc, nullptr, nullptr, DEBUG_INFO);
}

void lua_trace(bool)
{
    auto L = lua->lua_state();
    luaL_traceback(L, L, nullptr, 0);
    size_t len;

    gsl::czstring stack = lua_tolstring(L, -1, &len);
    if (stack == nullptr)
        return;

    xr_string data(stack, len);
    lua_pop(L, 1);

    Log(data);
}

class lua_scoped_handler
{
private:
    decltype(std::declval<xrDebug>().get_lua_panic()) old_panic{};
    decltype(std::declval<xrDebug>().get_lua_trace()) old_trace{};

public:
    constexpr lua_scoped_handler()
    {
        old_panic = Debug.set_lua_panic(lua_panic);
        old_trace = Debug.set_lua_trace(lua_trace);
    }

    constexpr ~lua_scoped_handler()
    {
        R_ASSERT(Debug.set_lua_trace(old_trace) == lua_trace);
        R_ASSERT(Debug.set_lua_panic(old_panic) == lua_panic);
    }
};

void do_file(gsl::czstring caScriptName, gsl::czstring caNameSpaceName)
{
    const auto l_tpFileReader = absl::WrapUnique(FS.r_open(caScriptName));
    if (!l_tpFileReader)
    {
        // заменить на ассерт?
        Msg("!![CResourceManager::do_file] Cannot open file [%s]", caScriptName);
        return;
    }

    l_tpFileReader->skip_bom(caScriptName);

    string_path l_caLuaFileName;
    xr_strconcat(l_caLuaFileName, "@", caScriptName); // KRodin: приводит путь к виду @f:\games\s.t.a.l.k.e.r\gamedata\scripts\class_registrator.script

    std::string_view strbuf{static_cast<gsl::czstring>(l_tpFileReader->pointer()), gsl::narrow_cast<size_t>(l_tpFileReader->elapsed())};
    xr_string script;

    if (std::is_neq(xr_strcmp(caNameSpaceName, GlobalNamespace)))
    {
        script = std::format(FILE_HEADER, caNameSpaceName, strbuf);
        strbuf = script;
    }

    lua->script(script, l_caLuaFileName);
}

[[nodiscard]] bool lua_function(gsl::czstring ns, gsl::czstring fn) { return !!lua->get<sol::optional<sol::function>>(std::tie(ns, fn)); }
} // namespace

// export
void CResourceManager::LS_Load()
{
    R_ASSERT(!lua, "! LuaJIT is already running");
    lua.emplace();
    R_ASSERT(lua, "! ERROR : Cannot initialize LUA VM!");

    lua->open_libraries();
    const lua_scoped_handler sc;

    lua->set_function("log", sol::resolve<void(gsl::czstring)>(&Log));

    lua->new_usertype<adopt_dx10sampler>("_dx10sampler", sol::no_constructor, sol::call_constructor, sol::constructors<adopt_dx10sampler(const adopt_dx10sampler&)>(), "clamp",
                                         sol::policies(&adopt_dx10sampler::_clamp, sol::returns_self()));

    lua->new_usertype<adopt_compiler>(
        "_compiler", sol::no_constructor, sol::call_constructor, sol::constructors<adopt_compiler(const adopt_compiler&)>(), "begin",
        sol::policies(sol::overload(&adopt_compiler::_pass, &adopt_compiler::_passgs), sol::returns_self()), "sorting",
        sol::policies(&adopt_compiler::_options, sol::returns_self()), "emissive", sol::policies(&adopt_compiler::_o_emissive, sol::returns_self()), "distort",
        sol::policies(&adopt_compiler::_o_distort, sol::returns_self()), "wmark", sol::policies(&adopt_compiler::_o_wmark, sol::returns_self()), "fog",
        sol::policies(&adopt_compiler::_fog, sol::returns_self()), "zb", sol::policies(&adopt_compiler::_ZB, sol::returns_self()), "blend",
        sol::policies(&adopt_compiler::_blend, sol::returns_self()), "aref", sol::policies(&adopt_compiler::_aref, sol::returns_self()), "dx10color_write_enable",
        sol::policies(&adopt_compiler::_dx10color_write_enable, sol::returns_self()), "color_write_enable",
        sol::policies(&adopt_compiler::_dx10color_write_enable, sol::returns_self()), "dx10texture", sol::policies(&adopt_compiler::_dx10texture, sol::returns_self()),
        "dx10stencil", sol::policies(&adopt_compiler::_dx10Stencil, sol::returns_self()), "dx10stencil_ref", sol::policies(&adopt_compiler::_dx10StencilRef, sol::returns_self()),
        "dx10cullmode", sol::policies(&adopt_compiler::_dx10CullMode, sol::returns_self()), "dx10zfunc", sol::policies(&adopt_compiler::_dx10ZFunc, sol::returns_self()),
        "dx10sampler", &adopt_compiler::_dx10sampler);

    lua->new_enum("blend", "zero", D3DBLEND_ZERO, "one", D3DBLEND_ONE, "srccolor", D3DBLEND_SRCCOLOR, "invsrccolor", D3DBLEND_INVSRCCOLOR, "srcalpha", D3DBLEND_SRCALPHA,
                  "invsrcalpha", D3DBLEND_INVSRCALPHA, "destalpha", D3DBLEND_DESTALPHA, "invdestalpha", D3DBLEND_INVDESTALPHA, "destcolor", D3DBLEND_DESTCOLOR, "invdestcolor",
                  D3DBLEND_INVDESTCOLOR, "srcalphasat", D3DBLEND_SRCALPHASAT);

    lua->new_enum("cmp_func", "never", D3DCMP_NEVER, "less", D3DCMP_LESS, "equal", D3DCMP_EQUAL, "lessequal", D3DCMP_LESSEQUAL, "greater", D3DCMP_GREATER, "notequal",
                  D3DCMP_NOTEQUAL, "greaterequal", D3DCMP_GREATEREQUAL, "always", D3DCMP_ALWAYS);

    lua->new_enum("stencil_op", "keep", D3DSTENCILOP_KEEP, "zero", D3DSTENCILOP_ZERO, "replace", D3DSTENCILOP_REPLACE, "incrsat", D3DSTENCILOP_INCRSAT, "decrsat",
                  D3DSTENCILOP_DECRSAT, "invert", D3DSTENCILOP_INVERT, "incr", D3DSTENCILOP_INCR, "decr", D3DSTENCILOP_DECR);

    // load shaders
    xr_vector<char*>* folder = FS.file_list_open("$game_shaders$", RImplementation.getShaderPath(), FS_ListFiles | FS_RootOnly);
    VERIFY(folder != nullptr);
    const auto _ = gsl::finally([&folder] { FS.file_list_close(folder); });

    for (const auto path : *folder)
    {
        string_path namesp, fn;
        xr_strcpy(namesp, path);

        auto ext = strext(namesp);
        if (ext == nullptr || std::is_neq(xr_strcmp(ext, ".s")))
            continue;

        *ext = '\0';
        if (namesp[0] == '\0')
            xr_strcpy(namesp, GlobalNamespace.data());

        xr_strconcat(fn, RImplementation.getShaderPath(), path);
        std::ignore = FS.update_path(fn, "$game_shaders$", fn);

        do_file(fn, namesp);
    }
}

void CResourceManager::LS_Unload() { lua.reset(); }

gsl::index CResourceManager::LS_mem() const { return lua ? gsl::narrow_cast<gsl::index>(lua->memory_used()) : 0z; }

BOOL CResourceManager::_lua_HasShader(LPCSTR s_shader)
{
    string256 undercorated;
    for (int i = 0, l = xr_strlen(s_shader) + 1; i < l; i++)
        undercorated[i] = ('\\' == s_shader[i]) ? '_' : s_shader[i];

    if (lua_function(undercorated, "normal") || lua_function(undercorated, "l_special"))
        return true;

    // If not found - try to find new ones
    for (gsl::index i{0}; i < SHADER_ELEMENTS_MAX; ++i)
    {
        string16 buff;
        std::snprintf(buff, sizeof(buff), "element_%zd", i);

        if (lua_function(undercorated, buff))
            return true;
    }

    return false;
}

Shader* CResourceManager::_lua_Create(LPCSTR d_shader, LPCSTR s_textures)
{
    CBlender_Compile C;
    Shader S;

    // undecorate
    string256 undercorated;
    for (int i = 0, l = xr_strlen(d_shader) + 1; i < l; i++)
        undercorated[i] = ('\\' == d_shader[i]) ? '_' : d_shader[i];
    LPCSTR s_shader = undercorated;

    // Access to template
    C.BT = nullptr;
    C.bDetail = FALSE;

    // Prepare
    _ParseList(C.L_textures, s_textures);
    C.detail_texture = nullptr;
    C.detail_scaler = nullptr;

    const lua_scoped_handler sc;
    // Choose workflow here: old (using named stages) or new (explicitly declaring stage number)
    bool bUseNewWorkflow{};

    for (auto [i, elem] : xr::views_enumerate(S.E))
    {
        string16 buff;
        std::snprintf(buff, sizeof(buff), "element_%zd", i);

        if (!lua_function(s_shader, buff))
            continue;

        C.iElement = i;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        elem._set(C._lua_Compile(s_shader, buff));

        bUseNewWorkflow = true;
    }

    if (!bUseNewWorkflow)
    {
        // Compile element	(LOD0 - HQ)
        if (lua_function(s_shader, "normal_hq"))
        {
            // Analyze possibility to detail this shader
            C.iElement = 0;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[0]._set(C._lua_Compile(s_shader, C.bDetail ? "normal_hq" : "normal"));
        }
        else if (lua_function(s_shader, "normal"))
        {
            C.iElement = 0;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[0]._set(C._lua_Compile(s_shader, "normal"));

            /// SSS fix water for DX10
            // Water Flag
            if (S.E[0]->flags.bDistort)
            {
                if (strstr(s_shader, "effects_water") != nullptr)
                    S.E[0]->flags.isWater = TRUE;
            }
        }

        // Compile element	(LOD1)
        if (lua_function(s_shader, "normal"))
        {
            C.iElement = 1;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[1]._set(C._lua_Compile(s_shader, "normal"));
        }

        // Compile element
        if (lua_function(s_shader, "l_point"))
        {
            C.iElement = 2;
            C.bDetail = FALSE;
            S.E[2]._set(C._lua_Compile(s_shader, "l_point"));
        }

        // Compile element
        if (lua_function(s_shader, "l_spot"))
        {
            C.iElement = 3;
            C.bDetail = FALSE;
            S.E[3]._set(C._lua_Compile(s_shader, "l_spot"));
        }

        // Compile element
        if (lua_function(s_shader, "l_special"))
        {
            C.iElement = 4;
            C.bDetail = FALSE;
            S.E[4]._set(C._lua_Compile(s_shader, "l_special"));
        }
    }

    // Search equal in shaders array
    for (auto sh : v_shaders)
    {
        if (S.equal(sh))
            return sh;
    }

    // Create _new_ entry
    Shader* N = v_shaders.emplace_back(xr_new<Shader>());
    N->clone(std::move(S));
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    return N;
}

ShaderElement* CBlender_Compile::_lua_Compile(LPCSTR namesp, LPCSTR name)
{
    ShaderElement E;
    SH = &E;
    RS.Invalidate();

    // Compile
    LPCSTR t_0 = *L_textures[0] ? *L_textures[0] : "null";
    LPCSTR t_1 = (L_textures.size() > 1) ? *L_textures[1] : "null";
    LPCSTR t_d = detail_texture ? detail_texture : "null";

    bool bFirstPass{};
    adopt_compiler ac{this, bFirstPass};
    sol::function element = (*lua)[namesp][name];

    element(ac, t_0, t_1, t_d);
    r_End();

    return RImplementation.Resources->_CreateElement(std::move(E));
}
