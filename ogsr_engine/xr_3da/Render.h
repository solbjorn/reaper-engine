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

class XR_NOVTABLE IRender_Light : public xr_resource
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
    [[nodiscard]] virtual u32 get_type() const = 0;
    virtual void set_active(bool) = 0;
    [[nodiscard]] virtual bool get_active() const = 0;
    virtual void set_shadow(bool) = 0;
    virtual void set_volumetric(bool, bool manual = false) = 0;
    virtual void set_volumetric_quality(f32) = 0;
    virtual void set_volumetric_intensity(f32) = 0;
    virtual void set_volumetric_distance(f32) = 0;
    virtual void set_indirect(bool) {}
    virtual void set_position(const Fvector3& P) = 0;
    virtual void set_rotation(const Fvector3& D, const Fvector3& R) = 0;
    virtual void set_cone(f32 angle) = 0;
    virtual void set_range(f32 R) = 0;
    [[nodiscard]] virtual f32 get_range() const = 0;
    virtual void set_virtual_size(f32 R) = 0;
    virtual void set_texture(gsl::czstring name) = 0;

    virtual void set_color(const Fcolor& C) = 0;
    virtual void set_color(f32 r, f32 g, f32 b) = 0;
    [[nodiscard]] virtual Fcolor get_color() const = 0;

    virtual void set_hud_mode(bool b) = 0;
    [[nodiscard]] virtual bool get_hud_mode() const = 0;

    virtual void set_moveable(bool) = 0;

    ~IRender_Light() override;
};

struct resptrcode_light : public resptr_base<IRender_Light>
{
    void destroy() { _set(nullptr); }
};

typedef resptr_core<IRender_Light, resptrcode_light> ref_light;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Glow)

class XR_NOVTABLE IRender_Glow : public xr_resource
{
    RTTI_DECLARE_TYPEINFO(IRender_Glow, xr_resource);

public:
    ~IRender_Glow() override;

    [[nodiscard]] virtual bool get_active() const = 0;
    virtual void set_active(bool) = 0;
    virtual void set_position(const Fvector& P) = 0;
    virtual void set_direction(const Fvector& P) = 0;
    virtual void set_radius(float R) = 0;
    virtual void set_texture(LPCSTR name) = 0;
    virtual void set_color(const Fcolor& C) = 0;
    virtual void set_color(float r, float g, float b) = 0;
};

struct resptrcode_glow : public resptr_base<IRender_Glow>
{
    void destroy() { _set(nullptr); }
};

typedef resptr_core<IRender_Glow, resptrcode_glow> ref_glow;

//////////////////////////////////////////////////////////////////////////
// definition (Per-object render-specific data)

class XR_NOVTABLE IRender_ObjectSpecific : public virtual RTTI::Enable
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

    virtual void force_mode(u32 mode) = 0;
    [[nodiscard]] virtual f32 get_luminocity() = 0;
    [[nodiscard]] virtual f32 get_luminocity_hemi() = 0;
    [[nodiscard]] virtual f32* get_luminocity_hemi_cube() = 0;

    ~IRender_ObjectSpecific() override = 0;
};

inline IRender_ObjectSpecific::~IRender_ObjectSpecific() = default;

//////////////////////////////////////////////////////////////////////////
// definition (Target)

class XR_NOVTABLE IRender_Target : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRender_Target);

public:
    virtual void set_blur(f32 f) = 0;
    virtual void set_gray(f32 f) = 0;
    virtual void set_duality_h(f32 f) = 0;
    virtual void set_duality_v(f32 f) = 0;
    virtual void set_noise(f32 f) = 0;
    virtual void set_noise_scale(f32 f) = 0;
    virtual void set_noise_fps(f32 f) = 0;
    virtual void set_color_base(u32 f) = 0;
    virtual void set_color_gray(u32 f) = 0;
    virtual void set_color_add(const Fvector3& f) = 0;
    virtual u32 get_width(ctx_id_t context_id) = 0;
    virtual u32 get_height(ctx_id_t context_id) = 0;
    virtual void set_cm_imfluence(f32 f) = 0;
    virtual void set_cm_interpolate(f32 f) = 0;
    virtual void set_cm_textures(const shared_str& tex0, const shared_str& tex1) = 0;

    ~IRender_Target() override = 0;
};

inline IRender_Target::~IRender_Target() = default;

//////////////////////////////////////////////////////////////////////////
// definition (Renderer)

enum class DeviceState
{
    Normal = 0,
    Lost,
    NeedReset
};

class XR_NOVTABLE IRender_interface : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRender_interface);

public:
    enum ScreenshotMode : u32
    {
        SM_NORMAL = 0, // jpeg,	name ignored
        SM_FOR_CUBEMAP = 1, // tga,		name used as postfix
        SM_FOR_GAMESAVE = 2, // dds/dxt1,name used as full-path
        SM_FOR_LEVELMAP = 3, // tga,		name used as postfix (level_name)
    };

    // options
    bool hud_loading{};
    s32 m_skinning;
    s32 m_MSAASample;

    // data
    CFrustum ViewBase;

    // Loading / Unloading
    virtual void create() = 0;
    virtual void destroy() = 0;
    virtual void reset_begin() = 0;
    virtual void reset_end() = 0;

    virtual tmc::task<void> level_Load(IReader* fs) = 0;
    virtual void level_Unload() = 0;

    void shader_option_skinning(s32 mode) { m_skinning = mode; }
    [[nodiscard]] virtual HRESULT shader_compile(gsl::czstring name, DWORD const* pSrcData, UINT SrcDataLen, gsl::czstring pFunctionName, gsl::czstring pTarget,
                                                 DWORD Flags, void*& result) = 0;

    // Information
    virtual void Statistics(CGameFont*) {}

    [[nodiscard]] virtual gsl::czstring getShaderPath() = 0;
    [[nodiscard]] virtual IRenderVisual* getVisual(s32 id) = 0;
    [[nodiscard]] virtual IRender_Target* getTarget() = 0;

    virtual void add_Visual(u32 context_id, IRenderable* root, IRenderVisual* V, Fmatrix& m) = 0; // add visual leaf	(no culling performed at all)
    virtual void add_StaticWallmark(const wm_shader& S, const Fvector3& P, f32 s, CDB::TRI* T, Fvector3* V) = 0;
    //	Prefer this function when possible
    virtual void add_StaticWallmark(IWallMarkArray* pArray, const Fvector3& P, f32 s, CDB::TRI* T, Fvector3* V) = 0;
    virtual void clear_static_wallmarks() = 0;
    //	Prefer this function when possible
    virtual void add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, const Fvector3& start, const Fvector3& dir, f32 size) = 0;

    [[nodiscard]] virtual IRender_ObjectSpecific* ros_create() = 0;
    virtual void ros_destroy(IRender_ObjectSpecific*&) = 0;

    // Lighting/glowing
    [[nodiscard]] virtual IRender_Light* light_create() = 0;
    virtual void light_destroy(IRender_Light*) {}
    [[nodiscard]] virtual IRender_Glow* glow_create() = 0;
    virtual void glow_destroy(IRender_Glow*) {}

    // Models
    [[nodiscard]] virtual IRenderVisual* model_CreateParticles(gsl::czstring name) = 0;
    [[nodiscard]] virtual IRenderVisual* model_Create(gsl::czstring name, IReader* data = nullptr) = 0;
    [[nodiscard]] virtual IRenderVisual* model_CreateChild(gsl::czstring name, IReader* data) = 0;
    [[nodiscard]] virtual IRenderVisual* model_Duplicate(IRenderVisual* V) = 0;

    virtual void model_Delete(IRenderVisual*& V, BOOL bDiscard = FALSE) = 0;
    virtual void model_Logging(BOOL bEnable) = 0;
    virtual void models_Prefetch() = 0;
    virtual void models_Clear(BOOL b_complete) = 0;
    virtual void models_savePrefetch() = 0;
    virtual void models_begin_prefetch1(bool val) = 0;

    // Occlusion culling
    [[nodiscard]] virtual BOOL occ_visible(vis_data& V) = 0;
    [[nodiscard]] virtual BOOL occ_visible(Fbox& B) = 0;
    [[nodiscard]] virtual BOOL occ_visible(sPoly& P) = 0;

    // Main
    virtual tmc::task<void> OnCameraUpdated() = 0;
    virtual tmc::task<void> Render() = 0;
    virtual void BeforeWorldRender() = 0; //--#SM+#-- Перед рендерингом мира
    virtual void AfterWorldRender(const bool save_bb_before_ui) = 0; //--#SM+#-- После рендеринга мира (до UI)
    virtual void AfterUIRender() = 0; // После рендеринга UI. Вызывать только если нам нужно отрендерить кадр для пда.

    virtual void Screenshot(ScreenshotMode mode = SM_NORMAL, gsl::czstring name = nullptr) = 0;

    // Constructor/destructor
    ~IRender_interface() override = 0;
};

inline IRender_interface::~IRender_interface() = default;

class XR_NOVTABLE ITexture : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ITexture);

public:
    ~ITexture() override = 0;

    virtual const char* GetName() const = 0;

    virtual void Load(const char* Name) = 0;
    virtual void Unload() = 0;
};

inline ITexture::~ITexture() = default;

class XR_NOVTABLE IResourceManager : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IResourceManager);

public:
    ~IResourceManager() override = 0;

    virtual xr_vector<ITexture*> FindTexture(const char* Name) const = 0;
};

inline IResourceManager::~IResourceManager() = default;

class ShExports final
{
    template <typename T, u32 Size>
    struct PositionsStorage
    {
        T Positions[Size]{};
        T& operator[](const u32& key)
        {
            ASSERT_FMT(key < std::size(Positions), "Out of range! key: [%u], size: [%zu]", key, std::size(Positions));
            return Positions[key];
        }
    };

    PositionsStorage<Fvector2, 24> artefacts_position{};
    PositionsStorage<Fvector2, 24> anomalys_position{};
    Ivector2 detector_params{};
    Fvector pda_params{}, actor_params{};

public:
    void set_artefact_position(const u32& _i, const Fvector2& _pos) { artefacts_position[_i] = _pos; }
    void set_anomaly_position(const u32& _i, const Fvector2& _pos) { anomalys_position[_i] = _pos; }
    void set_detector_params(const Ivector2& _pos) { detector_params = _pos; }
    void set_pda_params(const Fvector& _pos) { pda_params = _pos; }
    void set_actor_params(const Fvector& _pos) { actor_params = _pos; }

    const Fvector2& get_artefact_position(const u32& _i) { return artefacts_position[_i]; }
    const Fvector2& get_anomaly_position(const u32& _i) { return anomalys_position[_i]; }
    const Ivector2& get_detector_params() const { return detector_params; }
    const Fvector& get_pda_params() const { return pda_params; }
    const Fvector& get_actor_params() const { return actor_params; }
};

extern ShExports shader_exports;

// Увеличивая или уменьшая максимальное кол-во здесь, обязательно нужно сделать тоже самое в вершинном шейдере в объявлении benders_pos. Там должно быть это
// значение умноженное на два.
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

// dx10Texture.cpp
namespace xr
{
[[nodiscard]] bool texture_exists(string_path& fn, std::span<const std::string_view> places, std::string_view fname);
}

void fix_texture_name(gsl::zstring fn);

#endif
