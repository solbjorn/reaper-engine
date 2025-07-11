#ifndef _RENDER_H_
#define _RENDER_H_

#include "../xrCDB/frustum.h"

#include "vis_common.h"
// #include "IRenderDetailModel.h"

#include "../Include/xrAPI/xrAPI.h"
#include "../Include/xrRender/FactoryPtr.h"
class IUIShader;
typedef FactoryPtr<IUIShader> wm_shader;
// #include "../Include/xrRender/WallMarkArray.h"

// refs
class IRenderable;
// class IRenderVisual;

// class IBlender;
// class CSkeletonWallmark;
// class CKinematics;
struct FSlideWindowItem;

//	Igor
class IRenderVisual;
class IKinematics;
class CGameFont;
// class IRenderDetailModel;

extern const float fLightSmoothFactor;
extern int g_3dscopes_fps_factor;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Light)
class IRender_Light : public xr_resource
{
    RTTI_DECLARE_TYPEINFO(IRender_Light, xr_resource);

public:
    enum LT
    {
        DIRECT = 0,
        POINT = 1,
        SPOT = 2,
        OMNIPART = 3,
    };

public:
    virtual void set_type(LT type) = 0;
    virtual u32 get_type() const = 0;
    virtual void set_active(bool) = 0;
    virtual bool get_active() = 0;
    virtual void set_shadow(bool) = 0;
    virtual void set_volumetric(bool, bool manual = false) = 0;
    virtual void set_volumetric_quality(float) = 0;
    virtual void set_volumetric_intensity(float) = 0;
    virtual void set_volumetric_distance(float) = 0;
    virtual void set_indirect(bool) {};
    virtual void set_position(const Fvector& P) = 0;
    virtual void set_rotation(const Fvector& D, const Fvector& R) = 0;
    virtual void set_cone(float angle) = 0;
    virtual void set_range(float R) = 0;
    virtual float get_range() const = 0;
    virtual void set_virtual_size(float R) = 0;
    virtual void set_texture(LPCSTR name) = 0;

    virtual void set_color(const Fcolor& C) = 0;
    virtual void set_color(float r, float g, float b) = 0;
    virtual Fcolor get_color() const = 0;

    virtual void set_hud_mode(bool b) = 0;
    virtual bool get_hud_mode() = 0;

    virtual void set_moveable(bool) = 0;

    virtual ~IRender_Light();
};
struct resptrcode_light : public resptr_base<IRender_Light>
{
    void destroy() { _set(NULL); }
};
typedef resptr_core<IRender_Light, resptrcode_light> ref_light;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Glow)
class IRender_Glow : public xr_resource
{
    RTTI_DECLARE_TYPEINFO(IRender_Glow, xr_resource);

public:
    virtual void set_active(bool) = 0;
    virtual bool get_active() = 0;
    virtual void set_position(const Fvector& P) = 0;
    virtual void set_direction(const Fvector& P) = 0;
    virtual void set_radius(float R) = 0;
    virtual void set_texture(LPCSTR name) = 0;
    virtual void set_color(const Fcolor& C) = 0;
    virtual void set_color(float r, float g, float b) = 0;
    virtual ~IRender_Glow();
};
struct resptrcode_glow : public resptr_base<IRender_Glow>
{
    void destroy() { _set(NULL); }
};
typedef resptr_core<IRender_Glow, resptrcode_glow> ref_glow;

//////////////////////////////////////////////////////////////////////////
// definition (Per-object render-specific data)
class IRender_ObjectSpecific : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRender_ObjectSpecific);

public:
    enum mode
    {
        TRACE_LIGHTS = (1 << 0),
        TRACE_SUN = (1 << 1),
        TRACE_HEMI = (1 << 2),
        TRACE_ALL = (TRACE_LIGHTS | TRACE_SUN | TRACE_HEMI),
    };

public:
    virtual void force_mode(u32 mode) = 0;
    virtual float get_luminocity() = 0;
    virtual float get_luminocity_hemi() = 0;
    virtual float* get_luminocity_hemi_cube() = 0;

    virtual ~IRender_ObjectSpecific() {};
};

//////////////////////////////////////////////////////////////////////////
// definition (Target)
class IRender_Target : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRender_Target);

public:
    virtual void set_blur(float f) = 0;
    virtual void set_gray(float f) = 0;
    virtual void set_duality_h(float f) = 0;
    virtual void set_duality_v(float f) = 0;
    virtual void set_noise(float f) = 0;
    virtual void set_noise_scale(float f) = 0;
    virtual void set_noise_fps(float f) = 0;
    virtual void set_color_base(u32 f) = 0;
    virtual void set_color_gray(u32 f) = 0;
    virtual void set_color_add(const Fvector& f) = 0;
    virtual u32 get_width(ctx_id_t context_id) = 0;
    virtual u32 get_height(ctx_id_t context_id) = 0;
    virtual void set_cm_imfluence(float f) = 0;
    virtual void set_cm_interpolate(float f) = 0;
    virtual void set_cm_textures(const shared_str& tex0, const shared_str& tex1) = 0;
    virtual ~IRender_Target() {};
};

//////////////////////////////////////////////////////////////////////////
// definition (Renderer)

enum class DeviceState
{
    Normal = 0,
    Lost,
    NeedReset
};

class IRender_interface
{
public:
    enum ScreenshotMode
    {
        SM_NORMAL = 0, // jpeg,	name ignored
        SM_FOR_CUBEMAP = 1, // tga,		name used as postfix
        SM_FOR_GAMESAVE = 2, // dds/dxt1,name used as full-path
        SM_FOR_LEVELMAP = 3, // tga,		name used as postfix (level_name)
        SM_forcedword = u32(-1)
    };

public:
    // options
    bool hud_loading{};
    s32 m_skinning;
    s32 m_MSAASample;

    BENCH_SEC_SCRAMBLEMEMBER1

    // data
    CFrustum ViewBase;

public:
    // Loading / Unloading
    virtual void create() = 0;
    virtual void destroy() = 0;
    virtual void reset_begin() = 0;
    virtual void reset_end() = 0;

    BENCH_SEC_SCRAMBLEVTBL1
    BENCH_SEC_SCRAMBLEVTBL3

    virtual void level_Load(IReader*) = 0;
    virtual void level_Unload() = 0;

    void shader_option_skinning(s32 mode) { m_skinning = mode; }
    virtual HRESULT shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result) = 0;

    // Information
    virtual void Statistics(CGameFont* F) {};

    virtual LPCSTR getShaderPath() = 0;
    virtual IRenderVisual* getVisual(int id) = 0;
    virtual IRender_Target* getTarget() = 0;

    virtual void add_Visual(u32 context_id, IRenderable* root, IRenderVisual* V, Fmatrix& m) = 0; // add visual leaf	(no culling performed at all)
    virtual void add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V) = 0;
    //	Prefer this function when possible
    virtual void add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V) = 0;
    virtual void clear_static_wallmarks() = 0;
    // virtual void					add_SkeletonWallmark	(intrusive_ptr<CSkeletonWallmark> wm)						= 0;
    // virtual void					add_SkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)=0;
    //	Prefer this function when possible
    virtual void add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, const Fvector& start, const Fvector& dir, float size) = 0;

    // virtual IBlender*				blender_create			(CLASS_ID cls)								= 0;
    // virtual void					blender_destroy			(IBlender* &)								= 0;

    virtual IRender_ObjectSpecific* ros_create(IRenderable* parent) = 0;
    virtual void ros_destroy(IRender_ObjectSpecific*&) = 0;

    // Lighting/glowing
    virtual IRender_Light* light_create() = 0;
    virtual void light_destroy(IRender_Light* p_) {};
    virtual IRender_Glow* glow_create() = 0;
    virtual void glow_destroy(IRender_Glow* p_) {};

    // Models
    virtual IRenderVisual* model_CreateParticles(LPCSTR name) = 0;
    virtual IRenderVisual* model_Create(LPCSTR name, IReader* data = 0) = 0;
    virtual IRenderVisual* model_CreateChild(LPCSTR name, IReader* data) = 0;
    virtual IRenderVisual* model_Duplicate(IRenderVisual* V) = 0;

    virtual void model_Delete(IRenderVisual*& V, BOOL bDiscard = FALSE) = 0;
    virtual void model_Logging(BOOL bEnable) = 0;
    virtual void models_Prefetch() = 0;
    virtual void models_Clear(BOOL b_complete) = 0;
    virtual void models_savePrefetch() = 0;
    virtual void models_begin_prefetch1(bool val) = 0;

    // Occlusion culling
    virtual BOOL occ_visible(vis_data& V) = 0;
    virtual BOOL occ_visible(Fbox& B) = 0;
    virtual BOOL occ_visible(sPoly& P) = 0;

    // Main
    virtual void OnCameraUpdated() = 0;
    virtual void Calculate() = 0;
    virtual void Render() = 0;
    virtual void BeforeWorldRender() = 0; //--#SM+#-- Перед рендерингом мира
    virtual void AfterWorldRender(const bool save_bb_before_ui) = 0; //--#SM+#-- После рендеринга мира (до UI)
    virtual void AfterUIRender() = 0; // После рендеринга UI. Вызывать только если нам нужно отрендерить кадр для пда.

    virtual void Screenshot(ScreenshotMode mode = SM_NORMAL, LPCSTR name = 0) = 0;

    // Constructor/destructor
    virtual ~IRender_interface();

protected:
    virtual void ScreenshotImpl(ScreenshotMode mode, LPCSTR name, CMemoryWriter* memory_writer) = 0;
};

class ITexture : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ITexture);

public:
    virtual ~ITexture() = default;

    virtual const char* GetName() const = 0;

    virtual void Load(const char* Name) = 0;
    virtual void Unload() = 0;
};

class IResourceManager : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IResourceManager);

public:
    virtual ~IResourceManager() = default;

    virtual xr_vector<ITexture*> FindTexture(const char* Name) const = 0;
};

class ShExports final
{
    template <typename T, u32 Size>
    struct PositionsStorage
    {
        T Positions[Size]{};
        T& operator[](const u32& key)
        {
            ASSERT_FMT(key < std::size(Positions), "Out of range! key: [%u], size: [%u]", key, std::size(Positions));
            return Positions[key];
        }
    };

    PositionsStorage<Fvector2, 24> artefacts_position{};
    PositionsStorage<Fvector2, 24> anomalys_position{};
    Ivector2 detector_params{};
    Fvector pda_params{}, actor_params{};

public:
    void set_artefact_position(const u32& _i, const Fvector2& _pos) { artefacts_position[_i] = _pos; };
    void set_anomaly_position(const u32& _i, const Fvector2& _pos) { anomalys_position[_i] = _pos; };
    void set_detector_params(const Ivector2& _pos) { detector_params = _pos; };
    void set_pda_params(const Fvector& _pos) { pda_params = _pos; };
    void set_actor_params(const Fvector& _pos) { actor_params = _pos; };

    const Fvector2& get_artefact_position(const u32& _i) { return artefacts_position[_i]; }
    const Fvector2& get_anomaly_position(const u32& _i) { return anomalys_position[_i]; }
    const Ivector2& get_detector_params() const { return detector_params; }
    const Fvector& get_pda_params() const { return pda_params; }
    const Fvector& get_actor_params() const { return actor_params; }
};

extern ShExports shader_exports;

// Увеличивая или уменьшая максимальное кол-во здесь, обязательно нужно сделать тоже самое в вершинном шейдере в объявлении benders_pos. Там должно быть это значение умноженное на
// два.
constexpr size_t GRASS_SHADER_DATA_COUNT = 16;

struct alignas(32) GRASS_SHADER_DATA
{
    size_t index{};
    size_t pad{};
    s8 anim[GRASS_SHADER_DATA_COUNT]{};
    u16 id[GRASS_SHADER_DATA_COUNT]{};
    Fvector4 pos[GRASS_SHADER_DATA_COUNT]{}; // x,y,z - pos, w - radius_curr
    Fvector4 dir[GRASS_SHADER_DATA_COUNT]{}; // x,y,z - dir, w - str
    float radius[GRASS_SHADER_DATA_COUNT]{};
    float str_target[GRASS_SHADER_DATA_COUNT]{};
    float time[GRASS_SHADER_DATA_COUNT]{};
    float fade[GRASS_SHADER_DATA_COUNT]{};
    float speed[GRASS_SHADER_DATA_COUNT]{};
};
static_assert(offsetof(GRASS_SHADER_DATA, pos) == 64);

extern GRASS_SHADER_DATA grass_shader_data;

extern Fvector4 ps_ssfx_grass_interactive;
extern Fvector4 ps_ssfx_int_grass_params_2;

#endif
