#include "stdafx.h"

#include "../../xrScriptEngine/xr_sol.h"
#include "../../xr_3da/Render.h"

#include "../xrRender/ResourceManager.h"
#include "../xrRender/tss.h"
#include "../xrRender/blenders/blender.h"
#include "../xrRender/blenders/blender_recorder.h"
#include "../xrRender/dxRenderDeviceRender.h"

#include <format>

// wrapper
class adopt_dx10sampler
{
    CBlender_Compile* m_pC;
    u32 m_SI; //	Sampler index
public:
    adopt_dx10sampler(CBlender_Compile* C, u32 SamplerIndex) : m_pC(C), m_SI(SamplerIndex)
    {
        if (u32(-1) == m_SI)
            m_pC = 0;
    }
    adopt_dx10sampler(const adopt_dx10sampler& _C) : m_pC(_C.m_pC), m_SI(_C.m_SI)
    {
        if (u32(-1) == m_SI)
            m_pC = 0;
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

#pragma warning(push)
#pragma warning(disable : 4512)
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
    adopt_compiler(CBlender_Compile* _C, bool& bFirstPass) : C(_C), m_bFirstPass(bFirstPass) { m_bFirstPass = true; }
    adopt_compiler(const adopt_compiler& _C) : C(_C.C), m_bFirstPass(_C.m_bFirstPass) {}

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
#pragma warning(pop)

///////////////////////////////////////////////////////////////////////////////////////////////////////

static lua_State* LSVM = nullptr;

constexpr const char* GlobalNamespace = "_G";
static constexpr const char* FILE_HEADER =
    "\
local function script_name() \
return '{0}' \
end; \
local this; \
module('{0}', package.seeall, function(m) this = m end); \
{1}";

static bool do_file(sol::state_view lua, const char* caScriptName, const char* caNameSpaceName)
{
    auto l_tpFileReader = FS.r_open(caScriptName);
    if (!l_tpFileReader)
    {
        // заменить на ассерт?
        Msg("!![CResourceManager::do_file] Cannot open file [%s]", caScriptName);
        return false;
    }

    l_tpFileReader->skip_bom(caScriptName);

    string_path l_caLuaFileName;
    strconcat(sizeof(l_caLuaFileName), l_caLuaFileName, "@", caScriptName); // KRodin: приводит путь к виду @f:\games\s.t.a.l.k.e.r\gamedata\scripts\class_registrator.script

    const std::string_view strbuf{reinterpret_cast<const char*>(l_tpFileReader->pointer()), (size_t)l_tpFileReader->elapsed()};
    const std::string script = std::format(FILE_HEADER, caNameSpaceName, strbuf);

    FS.r_close(l_tpFileReader);

    try
    {
        lua.safe_script(script, l_caLuaFileName);
    }
    catch (const sol::error& e)
    {
        ASSERT_FMT(false, e.what());
        return false;
    }

    return true;
}

bool namespace_loaded(const char* name, bool remove_from_stack)
{
#ifdef DEBUG
    int start = lua_gettop(LSVM);
#endif
    lua_pushstring(LSVM, GlobalNamespace);
    lua_rawget(LSVM, LUA_GLOBALSINDEX);
    string256 S2;
    xr_strcpy(S2, name);
    auto S = S2;
    for (;;)
    {
        if (!xr_strlen(S))
        {
            VERIFY(lua_gettop(LSVM) >= 1);
            lua_pop(LSVM, 1);
            VERIFY(start == lua_gettop(LSVM));
            return false;
        }
        auto S1 = strchr(S, '.');
        if (S1)
            *S1 = 0;
        lua_pushstring(LSVM, S);
        lua_rawget(LSVM, -2);
        if (lua_isnil(LSVM, -1))
        {
            // lua_settop(LSVM,0);
            VERIFY(lua_gettop(LSVM) >= 2);
            lua_pop(LSVM, 2);
            VERIFY(start == lua_gettop(LSVM));
            return false; // there is no namespace!
        }
        else if (!lua_istable(LSVM, -1))
        {
            // lua_settop(LSVM, 0);
            VERIFY(lua_gettop(LSVM) >= 1);
            lua_pop(LSVM, 1);
            VERIFY(start == lua_gettop(LSVM));
            R_ASSERT3(false, "Error : the namespace is already being used by the non-table object! Name: ", S);
            return false;
        }
        lua_remove(LSVM, -2);
        if (S1)
            S = ++S1;
        else
            break;
    }
    if (!remove_from_stack)
        VERIFY(lua_gettop(LSVM) == start + 1);
    else
    {
        VERIFY(lua_gettop(LSVM) >= 1);
        lua_pop(LSVM, 1);
        VERIFY(lua_gettop(LSVM) == start);
    }
    return true;
}

bool OBJECT_1(const char* identifier, int type)
{
#ifdef DEBUG
    int start = lua_gettop(LSVM);
#endif
    lua_pushnil(LSVM);
    while (lua_next(LSVM, -2))
    {
        if (lua_type(LSVM, -1) == type && !xr_strcmp(identifier, lua_tostring(LSVM, -2)))
        {
            VERIFY(lua_gettop(LSVM) >= 3);
            lua_pop(LSVM, 3);
            VERIFY(lua_gettop(LSVM) == start - 1);
            return true;
        }
        lua_pop(LSVM, 1);
    }
    VERIFY(lua_gettop(LSVM) >= 1);
    lua_pop(LSVM, 1);
    VERIFY(lua_gettop(LSVM) == start - 1);
    return false;
}

bool OBJECT_2(const char* namespace_name, const char* identifier, int type)
{
#ifdef DEBUG
    int start = lua_gettop(LSVM);
#endif
    if (xr_strlen(namespace_name) && !namespace_loaded(namespace_name, false))
    {
        VERIFY(lua_gettop(LSVM) == start);
        return false;
    }
    bool result = OBJECT_1(identifier, type);
    VERIFY(lua_gettop(LSVM) == start);
    return result;
}

void LuaLog(const char* caMessage) { Log(caMessage); }

// export
void CResourceManager::LS_Load()
{
    //**************************************************************//
    // Msg("[CResourceManager] Starting LuaJIT");
    R_ASSERT2(!LSVM, "! LuaJIT is already running"); // На всякий случай

    LSVM = luaL_newstate(); // Запускаем LuaJIT. Память себе он выделит сам.
    R_ASSERT2(LSVM, "! ERROR : Cannot initialize LUA VM!"); // Надо проверить, случается ли такое.
    sol::set_default_state(LSVM);

    sol::state_view lua(LSVM);
    lua.open_libraries();

    lua.set_function("log", LuaLog);
    lua.new_usertype<adopt_dx10sampler>("_dx10sampler", sol::no_constructor, sol::call_constructor, sol::constructors<adopt_dx10sampler(const adopt_dx10sampler&)>(), "clamp",
                                        sol::policies(&adopt_dx10sampler::_clamp, sol::returns_self()));
    lua.new_usertype<adopt_compiler>(
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
    lua.new_enum("blend", "zero", D3DBLEND_ZERO, "one", D3DBLEND_ONE, "srccolor", D3DBLEND_SRCCOLOR, "invsrccolor", D3DBLEND_INVSRCCOLOR, "srcalpha", D3DBLEND_SRCALPHA,
                 "invsrcalpha", D3DBLEND_INVSRCALPHA, "destalpha", D3DBLEND_DESTALPHA, "invdestalpha", D3DBLEND_INVDESTALPHA, "destcolor", D3DBLEND_DESTCOLOR, "invdestcolor",
                 D3DBLEND_INVDESTCOLOR, "srcalphasat", D3DBLEND_SRCALPHASAT);
    lua.new_enum("cmp_func", "never", D3DCMP_NEVER, "less", D3DCMP_LESS, "equal", D3DCMP_EQUAL, "lessequal", D3DCMP_LESSEQUAL, "greater", D3DCMP_GREATER, "notequal",
                 D3DCMP_NOTEQUAL, "greaterequal", D3DCMP_GREATEREQUAL, "always", D3DCMP_ALWAYS);
    lua.new_enum("stencil_op", "keep", D3DSTENCILOP_KEEP, "zero", D3DSTENCILOP_ZERO, "replace", D3DSTENCILOP_REPLACE, "incrsat", D3DSTENCILOP_INCRSAT, "decrsat",
                 D3DSTENCILOP_DECRSAT, "invert", D3DSTENCILOP_INVERT, "incr", D3DSTENCILOP_INCR, "decr", D3DSTENCILOP_DECR);

    // load shaders
    xr_vector<char*>* folder = FS.file_list_open("$game_shaders$", RImplementation.getShaderPath(), FS_ListFiles | FS_RootOnly);
    VERIFY(folder);
    for (u32 it = 0; it < folder->size(); it++)
    {
        string_path namesp, fn;
        xr_strcpy(namesp, (*folder)[it]);
        if (0 == strext(namesp) || 0 != xr_strcmp(strext(namesp), ".s"))
            continue;
        *strext(namesp) = 0;
        if (0 == namesp[0])
            xr_strcpy(namesp, "_G");
        strconcat(sizeof(fn), fn, RImplementation.getShaderPath(), (*folder)[it]);
        FS.update_path(fn, "$game_shaders$", fn);
        do_file(lua, fn, namesp);
    }
    FS.file_list_close(folder);
}

void CResourceManager::LS_Unload()
{
    lua_close(LSVM);
    LSVM = nullptr;
}

BOOL CResourceManager::_lua_HasShader(LPCSTR s_shader)
{
    string256 undercorated;
    for (int i = 0, l = xr_strlen(s_shader) + 1; i < l; i++)
        undercorated[i] = ('\\' == s_shader[i]) ? '_' : s_shader[i];

    bool bHasShader = OBJECT_2(undercorated, "normal", LUA_TFUNCTION) || OBJECT_2(undercorated, "l_special", LUA_TFUNCTION);

    // If not found - try to find new ones
    if (!bHasShader)
    {
        for (int i = 0; i < SHADER_ELEMENTS_MAX; ++i)
        {
            string16 buff;
            std::snprintf(buff, sizeof(buff), "element_%d", i);
            if (OBJECT_2(undercorated, buff, LUA_TFUNCTION))
            {
                bHasShader = true;
                break;
            }
        }
    }
    return bHasShader;
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
    C.BT = NULL;
    C.bDetail = FALSE;

    // Prepare
    _ParseList(C.L_textures, s_textures);
    C.detail_texture = NULL;
    C.detail_scaler = NULL;

    // Choose workflow here: old (using named stages) or new (explicitly declaring stage number)
    bool bUseNewWorkflow = false;

    for (int i = 0; i < SHADER_ELEMENTS_MAX; ++i)
    {
        string16 buff;
        std::snprintf(buff, sizeof(buff), "element_%d", i);
        if (OBJECT_2(s_shader, buff, LUA_TFUNCTION))
        {
            C.iElement = i;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[i] = C._lua_Compile(s_shader, buff);

            bUseNewWorkflow = true;
        }
    }

    if (!bUseNewWorkflow)
    {
        // Compile element	(LOD0 - HQ)
        if (OBJECT_2(s_shader, "normal_hq", LUA_TFUNCTION))
        {
            // Analyze possibility to detail this shader
            C.iElement = 0;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);

            if (C.bDetail)
                S.E[0] = C._lua_Compile(s_shader, "normal_hq");
            else
                S.E[0] = C._lua_Compile(s_shader, "normal");
        }
        else
        {
            if (OBJECT_2(s_shader, "normal", LUA_TFUNCTION))
            {
                C.iElement = 0;
                C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
                S.E[0] = C._lua_Compile(s_shader, "normal");

                /// SSS fix water for DX10
                // Water Flag
                if (S.E[0]->flags.bDistort)
                {
                    if (strstr(s_shader, "effects_water"))
                        S.E[0]->flags.isWater = TRUE;
                }
            }
        }

        // Compile element	(LOD1)
        if (OBJECT_2(s_shader, "normal", LUA_TFUNCTION))
        {
            C.iElement = 1;
            C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
            S.E[1] = C._lua_Compile(s_shader, "normal");
        }

        // Compile element
        if (OBJECT_2(s_shader, "l_point", LUA_TFUNCTION))
        {
            C.iElement = 2;
            C.bDetail = FALSE;
            S.E[2] = C._lua_Compile(s_shader, "l_point");
        }

        // Compile element
        if (OBJECT_2(s_shader, "l_spot", LUA_TFUNCTION))
        {
            C.iElement = 3;
            C.bDetail = FALSE;
            S.E[3] = C._lua_Compile(s_shader, "l_spot");
        }

        // Compile element
        if (OBJECT_2(s_shader, "l_special", LUA_TFUNCTION))
        {
            C.iElement = 4;
            C.bDetail = FALSE;
            S.E[4] = C._lua_Compile(s_shader, "l_special");
        }
    }

    // Search equal in shaders array
    for (Shader* v_shader : v_shaders)
        if (S.equal(v_shader))
            return v_shader;

    // Create _new_ entry
    Shader* N = v_shaders.emplace_back(xr_new<Shader>(S));
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

    sol::function element = sol::state_view(LSVM)[namesp][name];

    bool bFirstPass = false;
    adopt_compiler ac = adopt_compiler(this, bFirstPass);
    element(ac, t_0, t_1, t_d);

    r_End();
    ShaderElement* _r = RImplementation.Resources->_CreateElement(std::move(E));
    return _r;
}
