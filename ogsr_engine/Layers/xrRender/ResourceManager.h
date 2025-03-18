// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ResourceManagerH
#define ResourceManagerH

#include "shader.h"
#include "tss_def.h"
#include "TextureDescrManager.h"

// refs
class dx10ConstantBuffer;

// defs
class CResourceManager : public IResourceManager
{
private:
    struct str_pred
    {
        IC bool operator()(LPCSTR x, LPCSTR y) const { return xr_strcmp(x, y) < 0; }
    };
    struct texture_detail
    {
        const char* T;
        R_constant_setup* cs;
    };

public:
    DEFINE_MAP_PRED(const char*, IBlenderXr*, map_Blender, map_BlenderIt, pred_str);
    DEFINE_MAP_PRED(const char*, CTexture*, map_Texture, map_TextureIt, pred_str);
    DEFINE_MAP_PRED(const char*, CRT*, map_RT, map_RTIt, pred_str);
    DEFINE_MAP_PRED(const char*, SVS*, map_VS, map_VSIt, pred_str);
    DEFINE_MAP_PRED(const char*, SGS*, map_GS, map_GSIt, pred_str);
    DEFINE_MAP_PRED(const char*, SHS*, map_HS, map_HSIt, pred_str);
    DEFINE_MAP_PRED(const char*, SDS*, map_DS, map_DSIt, pred_str);
    DEFINE_MAP_PRED(const char*, SCS*, map_CS, map_CSIt, pred_str);
    DEFINE_MAP_PRED(const char*, SPS*, map_PS, map_PSIt, pred_str);
    DEFINE_MAP_PRED(const char*, texture_detail, map_TD, map_TDIt, pred_str);

private:
    // data
    map_Blender m_blenders;
    map_Texture m_textures;
    map_RT m_rtargets;

    map_VS m_vs;
    map_PS m_ps;
    map_GS m_gs;
    map_DS m_ds;
    map_HS m_hs;
    map_CS m_cs;

    xr_vector<SState*> v_states;
    xr_vector<SDeclaration*> v_declarations;
    xr_vector<SGeometry*> v_geoms;
    xr_vector<R_constant_table*> v_constant_tables;

    xr_vector<dx10ConstantBuffer*> v_constant_buffer[R__NUM_CONTEXTS];
    xr_vector<SInputSignature*> v_input_signature;

    // lists
    xr_vector<STextureList*> lst_textures;
    xr_vector<SMatrixList*> lst_matrices;
    xr_vector<SConstantList*> lst_constants;

    // main shader-array
    xr_vector<SPass*> v_passes;
    xr_vector<ShaderElement*> v_elements;
    xr_vector<Shader*> v_shaders;
    // misc
public:
    CTextureDescrMngr m_textures_description;
    xr_vector<std::pair<shared_str, R_constant_setup*>> v_constant_setup;
    BOOL bDeferredLoad;

private:
    void LS_Load();
    void LS_Unload();

    // Miscelaneous
    void _ParseList(sh_list& dest, LPCSTR names);
    IBlender* _GetBlender(LPCSTR Name);

    Shader* _cpp_Create(LPCSTR s_shader, LPCSTR s_textures = 0, LPCSTR s_constants = 0, LPCSTR s_matrices = 0);
    Shader* _cpp_Create(IBlender* B, LPCSTR s_shader = 0, LPCSTR s_textures = 0, LPCSTR s_constants = 0, LPCSTR s_matrices = 0);

    Shader* _lua_Create(LPCSTR s_shader, LPCSTR s_textures);
    static BOOL _lua_HasShader(LPCSTR s_shader);

    void LoadShaderFile(LPCSTR name);
    void LoadShaderLtxFile(LPCSTR name);

public:
    CResourceManager() : bDeferredLoad(TRUE) {}
    ~CResourceManager();

    void _GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
    void _DumpMemoryUsage();

    // Debug
    void DBG_VerifyTextures();

    // Low level resource creation
    CTexture* _CreateTexture(LPCSTR Name);
    void _DeleteTexture(const CTexture* T);

    R_constant_table* _CreateConstantTable(R_constant_table& C);
    void _DeleteConstantTable(const R_constant_table* C);

    dx10ConstantBuffer* _CreateConstantBuffer(ctx_id_t context_id, ID3DShaderReflectionConstantBuffer* pTable);
    void _DeleteConstantBuffer(ctx_id_t context_id, const dx10ConstantBuffer* pBuffer);

    SInputSignature* _CreateInputSignature(ID3DBlob* pBlob);
    void _DeleteInputSignature(const SInputSignature* pSignature);

    CRT* _CreateRT(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 SampleCount = 1, u32 slices_num = 1, Flags32 flags = {});
    void _DeleteRT(const CRT* RT);

    SGS* _CreateGS(LPCSTR Name);
    void _DeleteGS(const SGS* GS);

    SHS* _CreateHS(LPCSTR Name);
    void _DeleteHS(const SHS* HS);

    SDS* _CreateDS(LPCSTR Name);
    void _DeleteDS(const SDS* DS);

    SCS* _CreateCS(LPCSTR Name);
    void _DeleteCS(const SCS* CS);

    SPS* _CreatePS(LPCSTR Name);
    void _DeletePS(const SPS* PS);

    SVS* _CreateVS(LPCSTR Name);
    void _DeleteVS(const SVS* VS);

    SPass* _CreatePass(const SPass& proto);
    void _DeletePass(const SPass* P);

    // Shader compiling / optimizing
    SState* _CreateState(SimulatorStates& Code);
    void _DeleteState(const SState* SB);

    SDeclaration* _CreateDecl(const D3DVERTEXELEMENT9* dcl);
    void _DeleteDecl(const SDeclaration* dcl);

    STextureList* _CreateTextureList(STextureList& L);
    void _DeleteTextureList(const STextureList* L);

    void _DeleteMatrixList(const SMatrixList* L);

    SConstantList* _CreateConstantList(SConstantList& L);
    void _DeleteConstantList(const SConstantList* L);

    ShaderElement* _CreateElement(ShaderElement&& L);
    void _DeleteElement(const ShaderElement* L);

    void OnDeviceCreate();
    void OnDeviceDestroy(BOOL bKeepTextures);

    void reset_begin();
    void reset_end();

    // Creation/Destroying
    Shader* Create(LPCSTR s_shader = nullptr, LPCSTR s_textures = nullptr, LPCSTR s_constants = nullptr, LPCSTR s_matrices = nullptr);
    Shader* Create(IBlender* B, LPCSTR s_shader = nullptr, LPCSTR s_textures = nullptr, LPCSTR s_constants = nullptr, LPCSTR s_matrices = nullptr);
    void Delete(const Shader* S);

    void RegisterConstantSetup(LPCSTR name, R_constant_setup* s) { v_constant_setup.emplace_back(shared_str(name), s); }

    SGeometry* CreateGeom(const D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
    SGeometry* CreateGeom(u32 FVF, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib);
    void DeleteGeom(const SGeometry* VS);

    void DeferredLoad(BOOL E) { bDeferredLoad = E; }
    void DeferredUpload();
    void Evict();
    void Dump(bool bBrief);

    xr_vector<ITexture*> FindTexture(const char* Name) const override;

private:
    template <typename T>
    T& GetShaderMap();

    template <typename T>
    T* CreateShader(const char* name);

    template <typename T>
    void DestroyShader(const T* sh);

    template <typename T>
    bool reclaim(xr_vector<T*>& vec, const T* ptr)
    {
        auto it = vec.begin();
        const auto end = vec.cend();

        for (; it != end; ++it)
        {
            if (*it == ptr)
            {
                vec.erase(it);
                return true;
            }
        }
        return false;
    }
};

#endif // ResourceManagerH
