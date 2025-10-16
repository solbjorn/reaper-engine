#include "stdafx.h"
#include "dxEnvironmentRender.h"

#include "blenders/blender.h"
#include "ResourceManager.h"

#include "../../xr_3da/Environment.h"
#include "../../xr_3da/xr_efflensflare.h"

//////////////////////////////////////////////////////////////////////////
// half box def
static constexpr Fvector3 hbox_verts[24] = {
    {-1.f, -1.f, -1.f},  {-1.f, -1.01f, -1.f}, // down
    {1.f, -1.f, -1.f},   {1.f, -1.01f, -1.f}, // down
    {-1.f, -1.f, 1.f},   {-1.f, -1.01f, 1.f}, // down
    {1.f, -1.f, 1.f},    {1.f, -1.01f, 1.f}, // down
    {-1.f, 1.f, -1.f},   {-1.f, 1.f, -1.f},    {1.f, 1.f, -1.f}, {1.f, 1.f, -1.f},     {-1.f, 1.f, 1.f},
    {-1.f, 1.f, 1.f},    {1.f, 1.f, 1.f},      {1.f, 1.f, 1.f},  {-1.f, -0.01f, -1.f}, {-1.f, -1.f, -1.f}, // half
    {1.f, -0.01f, -1.f}, {1.f, -1.f, -1.f}, // half
    {1.f, -0.01f, 1.f},  {1.f, -1.f, 1.f}, // half
    {-1.f, -0.01f, 1.f}, {-1.f, -1.f, 1.f} // half
};

static constexpr u16 __declspec(align(8)) hbox_faces[20 * 3] = {0, 2,  3, 3, 1, 0, 4, 5, 7,  7,  6,  4, 0,  1,  9, 9, 8, 0,  8, 9, 5, 5, 4,  8, 1,  3, 10, 10, 9, 1,
                                                                9, 10, 7, 7, 5, 9, 3, 2, 11, 11, 10, 3, 10, 11, 6, 6, 7, 10, 2, 0, 8, 8, 11, 2, 11, 8, 4,  4,  6, 11};

struct alignas(8) v_skybox
{
    Fvector3 p;
    u32 color;
    Fvector3 uv[2];

    void set(const Fvector3& _p, u32 _c, const Fvector3& _tc)
    {
        p = _p;
        color = _c;
        uv[0] = _tc;
        uv[1] = _tc;
    }
};
static_assert(sizeof(v_skybox) == 40);
constexpr u32 v_skybox_fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1);

struct v_clouds
{
    Fvector3 p;
    u32 color;
    u32 intensity;

    void set(const Fvector3& _p, u32 _c, u32 _i)
    {
        p = _p;
        color = _c;
        intensity = _i;
    }
};
static_assert(sizeof(v_clouds) == 20);
constexpr u32 v_clouds_fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;

class CBlender_skybox : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_skybox, IBlender);

public:
    const char* getComment() override { return "INTERNAL: combiner"; }

    void Compile(CBlender_Compile& C) override
    {
        C.r_Pass("sky2", "sky2", FALSE, TRUE, FALSE);
        C.r_dx10Texture("s_sky0", "$null");
        C.r_dx10Texture("s_sky1", "$null");
        C.r_dx10Texture("s_tonemap", "$user$tonemap");
        C.r_dx10Sampler("smp_rtlinear");
        C.PassSET_ZB(FALSE, FALSE);
        C.r_End();
    }
};

void dxEnvDescriptorRender::Copy(IEnvDescriptorRender& _in)
{
    auto& in{*smart_cast<const dxEnvDescriptorRender*>(&_in)};

    sky_texture = in.sky_texture;
    sky_texture_env = in.sky_texture_env;
    clouds_texture = in.clouds_texture;
}

void dxEnvironmentRender::Copy(IEnvironmentRender& _in)
{
    auto& in{*smart_cast<const dxEnvironmentRender*>(&_in)};

    sky_r_textures.clone(in.sky_r_textures);
    clouds_r_textures.clone(in.clouds_r_textures);

    sh_2sky = in.sh_2sky;
    sh_2geom = in.sh_2geom;
    clouds_sh = in.clouds_sh;
    clouds_geom = in.clouds_geom;

    tsky0 = in.tsky0;
    tsky1 = in.tsky1;
    t_envmap_0 = in.t_envmap_0;
    t_envmap_1 = in.t_envmap_1;
    tonemap = in.tonemap;

    tsky0_tstage = in.tsky0_tstage;
    tsky1_tstage = in.tsky1_tstage;
    tclouds0_tstage = in.tclouds0_tstage;
    tclouds1_tstage = in.tclouds1_tstage;

    tonemap_tstage_2sky = in.tonemap_tstage_2sky;
    tonemap_tstage_clouds = in.tonemap_tstage_clouds;
}

void dxEnvDescriptorRender::OnDeviceCreate(CEnvDescriptor& owner)
{
    if (owner.sky_texture_name.size())
        sky_texture.create(owner.sky_texture_name.c_str());

    if (owner.sky_texture_env_name.size())
        sky_texture_env.create(owner.sky_texture_env_name.c_str());

    if (owner.clouds_texture_name.size())
        clouds_texture.create(owner.clouds_texture_name.c_str());
}

void dxEnvDescriptorRender::OnDeviceDestroy()
{
    sky_texture.destroy();
    sky_texture_env.destroy();
    clouds_texture.destroy();
}

dxEnvironmentRender::dxEnvironmentRender()
{
    tsky0.create(r2_T_sky0);
    tsky1.create(r2_T_sky1);
    t_envmap_0.create(r2_T_envs0);
    t_envmap_1.create(r2_T_envs1);
    tonemap.create(r2_RT_luminance_cur);
}

void dxEnvironmentRender::Clear()
{
    std::pair<u32, ref_texture> zero = std::make_pair(u32(0), ref_texture(nullptr));

    sky_r_textures.clear();
    sky_r_textures.push_back(zero);
    sky_r_textures.push_back(zero);
    sky_r_textures.push_back(zero);

    clouds_r_textures.clear();
    clouds_r_textures.push_back(zero);
    clouds_r_textures.push_back(zero);
    clouds_r_textures.push_back(zero);
}

void dxEnvironmentRender::lerp(IEnvDescriptorRender* inA, IEnvDescriptorRender* inB)
{
    dxEnvDescriptorRender* pA = (dxEnvDescriptorRender*)inA;
    dxEnvDescriptorRender* pB = (dxEnvDescriptorRender*)inB;

    sky_r_textures.clear();
    sky_r_textures.emplace_back(tsky0_tstage, pA->sky_texture);
    sky_r_textures.emplace_back(tsky1_tstage, pB->sky_texture);
    sky_r_textures.emplace_back(tonemap_tstage_2sky, tonemap);

    clouds_r_textures.clear();
    clouds_r_textures.emplace_back(tclouds0_tstage, pA->clouds_texture);
    clouds_r_textures.emplace_back(tclouds1_tstage, pB->clouds_texture);
    clouds_r_textures.emplace_back(tonemap_tstage_clouds, tonemap);

    auto e0 = sky_r_textures[0].second->surface_get();
    auto e1 = sky_r_textures[1].second->surface_get();
    tsky0->surface_set(e0);
    tsky1->surface_set(e1);

    const bool menu_pp = g_pGamePersistent->OnRenderPPUI_query();

    e0 = menu_pp ? nullptr : pA->sky_texture_env->surface_get();
    e1 = menu_pp ? nullptr : pB->sky_texture_env->surface_get();
    t_envmap_0->surface_set(e0);
    t_envmap_1->surface_set(e1);
}

void dxEnvironmentRender::RenderSky(CEnvironment& env)
{
    RImplementation.rmFar(RCache);

    // draw sky box
    Fmatrix mSky;
    mSky.rotateY(env.CurrentEnv->sky_rotation);
    mSky.translate_over(Device.vCameraPosition);

    u32 i_offset, v_offset;
    u32 C = color_rgba(iFloor(env.CurrentEnv->sky_color.x * 255.f), iFloor(env.CurrentEnv->sky_color.y * 255.f), iFloor(env.CurrentEnv->sky_color.z * 255.f),
                       iFloor(env.CurrentEnv->weight * 255.f));

    // Fill index buffer
    u16* pib = RImplementation.Index.Lock(20 * 3, i_offset);
    CopyMemory(pib, hbox_faces, sizeof(hbox_faces));
    RImplementation.Index.Unlock(20 * 3);

    // Fill vertex buffer
    v_skybox* pv = (v_skybox*)RImplementation.Vertex.Lock(12, sh_2geom.stride(), v_offset);
    for (u32 v = 0; v < 12; v++)
    {
        Fvector3 hv = hbox_verts[v * 2];
        if (v >= 4 && v <= 7)
            hv.y *= env.CurrentEnv->sky_height;
        pv[v].set(hv, C, hbox_verts[v * 2 + 1]);
    }
    RImplementation.Vertex.Unlock(12, sh_2geom.stride());

    // Render
    RCache.set_xform_world(mSky);
    RCache.set_Geometry(sh_2geom);
    RCache.set_Shader(sh_2sky);
    RCache.set_Textures(&sky_r_textures);
    RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, 12, i_offset, 20);

    // Sun
    RImplementation.rmNormal(RCache);

    //
    // This hack is done to make sure that the state is set for sure:
    // The state may be not set by RCache if the state is changed using API SetRenderState() function before
    // and the RCache flag will remain unchanged to it's old value.
    //
    RCache.set_Z(FALSE);
    RCache.set_Z(TRUE);
    env.eff_LensFlare->Render(TRUE, FALSE, FALSE);
    RCache.set_Z(FALSE);
}

void dxEnvironmentRender::RenderClouds(CEnvironment& env)
{
    RImplementation.rmFar(RCache);

    Fmatrix mXFORM, mScale;
    mScale.scale(10, 0.4f, 10);
    mXFORM.rotateY(env.CurrentEnv->clouds_rotation);
    mXFORM.mulB_43(mScale);
    mXFORM.translate_over(Device.vCameraPosition);

    Fvector wd0, wd1;
    Fvector4 wind_dir;
    wd0.setHP(PI_DIV_4, 0);
    wd1.setHP(PI_DIV_4 + PI_DIV_8, 0);
    wind_dir.set(wd0.x, wd0.z, wd1.x, wd1.z).mul(0.5f).add(0.5f).mul(255.f);
    u32 i_offset, v_offset;
    u32 C0 = color_rgba(iFloor(wind_dir.x), iFloor(wind_dir.y), iFloor(wind_dir.w), iFloor(wind_dir.z));
    u32 C1 = color_rgba(iFloor(env.CurrentEnv->clouds_color.x * 255.f), iFloor(env.CurrentEnv->clouds_color.y * 255.f), iFloor(env.CurrentEnv->clouds_color.z * 255.f),
                        iFloor(env.CurrentEnv->clouds_color.w * 255.f));

    // Fill index buffer
    u16* pib = RImplementation.Index.Lock(env.CloudsIndices.size(), i_offset);
    CopyMemory(pib, &env.CloudsIndices.front(), env.CloudsIndices.size() * sizeof(u16));
    RImplementation.Index.Unlock(env.CloudsIndices.size());

    // Fill vertex buffer
    v_clouds* pv = (v_clouds*)RImplementation.Vertex.Lock(env.CloudsVerts.size(), clouds_geom.stride(), v_offset);
    for (FvectorIt it = env.CloudsVerts.begin(); it != env.CloudsVerts.end(); it++, pv++)
        pv->set(*it, C0, C1);
    RImplementation.Vertex.Unlock(env.CloudsVerts.size(), clouds_geom.stride());

    // Render
    RCache.set_xform_world(mXFORM);
    RCache.set_Geometry(clouds_geom);
    RCache.set_Shader(clouds_sh);
    RCache.set_Textures(&clouds_r_textures);
    RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, env.CloudsVerts.size(), i_offset, env.CloudsIndices.size() / 3);

    RImplementation.rmNormal(RCache);
}

void dxEnvironmentRender::OnDeviceCreate()
{
    sh_2sky.create<CBlender_skybox>("skybox_2t");
    sh_2geom.create(v_skybox_fvf, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());
    clouds_sh.create("clouds", "null");
    clouds_geom.create(v_clouds_fvf, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());

    const auto& sky2_constants = sh_2sky->E[0]->passes[0]->constants;
    const auto& clouds_constants = clouds_sh->E[0]->passes[0]->constants;

    // Just let texture stages be 0 if constants are missing
    if (sky2_constants)
    {
        if (const auto C = sky2_constants->get("s_sky0")._get())
            tsky0_tstage = C->samp.index;

        if (const auto C = sky2_constants->get("s_sky1")._get())
            tsky1_tstage = C->samp.index;
    }

    if (clouds_constants)
    {
        if (const auto C = clouds_constants->get("s_clouds0")._get())
            tclouds0_tstage = C->samp.index;

        if (const auto C = clouds_constants->get("s_clouds1")._get())
            tclouds1_tstage = C->samp.index;
    }

    tonemap_tstage_2sky = sh_2sky->E[0]->passes[0]->T->find_texture_stage(r2_RT_luminance_cur);
    tonemap_tstage_clouds = clouds_sh->E[0]->passes[0]->T->find_texture_stage(r2_RT_luminance_cur);
}

void dxEnvironmentRender::OnDeviceDestroy()
{
    sky_r_textures.clear();
    clouds_r_textures.clear();

    tsky0->surface_set(nullptr);
    tsky1->surface_set(nullptr);
    t_envmap_0->surface_set(nullptr);
    t_envmap_1->surface_set(nullptr);
    tonemap->surface_set(nullptr);

    sh_2sky.destroy();
    sh_2geom.destroy();
    clouds_sh.destroy();
    clouds_geom.destroy();

    tsky0_tstage = 0;
    tsky1_tstage = 0;
    tclouds0_tstage = 0;
    tclouds1_tstage = 0;
    tonemap_tstage_2sky = u32(-1);
    tonemap_tstage_clouds = u32(-1);
}
