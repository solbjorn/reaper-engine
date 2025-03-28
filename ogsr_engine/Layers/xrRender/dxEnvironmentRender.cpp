#include "stdafx.h"
#include "dxEnvironmentRender.h"

#include "dxRenderDeviceRender.h"

#include "../../xr_3da/environment.h"
#include "../../Layers/xrRender/ResourceManager.h"

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

static constexpr u16 hbox_faces[20 * 3] = {0, 2,  3, 3, 1, 0, 4, 5, 7,  7,  6,  4, 0,  1,  9, 9, 8, 0,  8, 9, 5, 5, 4,  8, 1,  3, 10, 10, 9, 1,
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

void dxEnvDescriptorRender::Copy(IEnvDescriptorRender& _in) { *this = *(dxEnvDescriptorRender*)&_in; }

void dxEnvDescriptorMixerRender::Copy(IEnvDescriptorMixerRender& _in) { *this = *(dxEnvDescriptorMixerRender*)&_in; }

void dxEnvironmentRender::Copy(IEnvironmentRender& _in) { *this = *(dxEnvironmentRender*)&_in; }

void dxEnvDescriptorMixerRender::Destroy()
{
    sky_r_textures.clear();
    sky_r_textures_env.clear();
    clouds_r_textures.clear();
}

void dxEnvDescriptorMixerRender::Clear()
{
    std::pair<u32, ref_texture> zero = mk_pair(u32(0), ref_texture(0));
    sky_r_textures.clear();
    sky_r_textures.push_back(zero);
    sky_r_textures.push_back(zero);
    sky_r_textures.push_back(zero);

    sky_r_textures_env.clear();
    sky_r_textures_env.push_back(zero);
    sky_r_textures_env.push_back(zero);
    sky_r_textures_env.push_back(zero);

    clouds_r_textures.clear();
    clouds_r_textures.push_back(zero);
    clouds_r_textures.push_back(zero);
    clouds_r_textures.push_back(zero);
}

void dxEnvDescriptorMixerRender::lerp(IEnvDescriptorRender* inA, IEnvDescriptorRender* inB)
{
    dxEnvDescriptorRender* pA = (dxEnvDescriptorRender*)inA;
    dxEnvDescriptorRender* pB = (dxEnvDescriptorRender*)inB;

    sky_r_textures.clear();
    sky_r_textures.emplace_back(0, pA->sky_texture);
    sky_r_textures.emplace_back(1, pB->sky_texture);

    sky_r_textures_env.clear();
    sky_r_textures_env.emplace_back(0, pA->sky_texture_env);
    sky_r_textures_env.emplace_back(1, pB->sky_texture_env);

    clouds_r_textures.clear();
    clouds_r_textures.emplace_back(0, pA->clouds_texture);
    clouds_r_textures.emplace_back(1, pB->clouds_texture);
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
    tsky0 = DEV->_CreateTexture("$user$sky0");
    tsky1 = DEV->_CreateTexture("$user$sky1");
}

void dxEnvironmentRender::OnFrame(CEnvironment& env)
{
    dxEnvDescriptorMixerRender& mixRen = *(dxEnvDescriptorMixerRender*)&*env.CurrentEnv->m_pDescriptorMixer;

    // tonemapping in VS
    mixRen.sky_r_textures.emplace_back(u32(D3DVERTEXTEXTURESAMPLER0), tonemap); //. hack
    mixRen.sky_r_textures_env.emplace_back(u32(D3DVERTEXTEXTURESAMPLER0), tonemap); //. hack
    mixRen.clouds_r_textures.emplace_back(u32(D3DVERTEXTEXTURESAMPLER0), tonemap); //. hack

    //. Setup skybox textures, somewhat ugly
    ID3DBaseTexture* e0 = mixRen.sky_r_textures[0].second->surface_get();
    ID3DBaseTexture* e1 = mixRen.sky_r_textures[1].second->surface_get();

    tsky0->surface_set(e0);
    tsky1->surface_set(e1);
}

void dxEnvironmentRender::OnLoad()
{
    tonemap = DEV->_CreateTexture("$user$tonemap"); //. hack
}

void dxEnvironmentRender::OnUnload() { tonemap = 0; }

void dxEnvironmentRender::RenderSky(CEnvironment& env)
{
    // clouds_sh.create		("clouds","null");
    //. this is the bug-fix for the case when the sky is broken
    //. for some unknown reason the geoms happen to be invalid sometimes
    //. if vTune show this in profile, please add simple cache (move-to-forward last found)
    //. to the following functions:
    //.		CResourceManager::_CreateDecl
    //.		CResourceManager::CreateGeom
    if (env.bNeed_re_create_env)
    {
        sh_2sky.create(&m_b_skybox, "skybox_2t");
        sh_2geom.create(v_skybox_fvf, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());
        clouds_sh.create("clouds", "null");
        clouds_geom.create(v_clouds_fvf, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());
        env.bNeed_re_create_env = FALSE;
    }
    RImplementation.rmFar(RCache);

    dxEnvDescriptorMixerRender& mixRen = *(dxEnvDescriptorMixerRender*)&*env.CurrentEnv->m_pDescriptorMixer;

    // draw sky box
    Fmatrix mSky;
    mSky.rotateY(env.CurrentEnv->sky_rotation);
    mSky.translate_over(Device.vCameraPosition);

    u32 i_offset, v_offset;
    u32 C = color_rgba(iFloor(env.CurrentEnv->sky_color.x * 255.f), iFloor(env.CurrentEnv->sky_color.y * 255.f), iFloor(env.CurrentEnv->sky_color.z * 255.f),
                       iFloor(env.CurrentEnv->weight * 255.f));

    // Fill index buffer
    u16* pib = RImplementation.Index.Lock(20 * 3, i_offset);
    CopyMemory(pib, hbox_faces, sizeof hbox_faces);
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
    //	RCache.set_Textures			(&env.CurrentEnv->sky_r_textures);
    RCache.set_Textures(&mixRen.sky_r_textures);
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
    mXFORM.rotateY(env.CurrentEnv->sky_rotation);
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
    dxEnvDescriptorMixerRender& mixRen = *(dxEnvDescriptorMixerRender*)&*env.CurrentEnv->m_pDescriptorMixer;
    RCache.set_Textures(&mixRen.clouds_r_textures);
    RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, env.CloudsVerts.size(), i_offset, env.CloudsIndices.size() / 3);

    RImplementation.rmNormal(RCache);
}

void dxEnvironmentRender::OnDeviceCreate()
{
    sh_2sky.create(&m_b_skybox, "skybox_2t");
    sh_2geom.create(v_skybox_fvf, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());
    clouds_sh.create("clouds", "null");
    clouds_geom.create(v_clouds_fvf, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());
}

void dxEnvironmentRender::OnDeviceDestroy()
{
    tsky0->surface_set(NULL);
    tsky1->surface_set(NULL);

    sh_2sky.destroy();
    sh_2geom.destroy();
    clouds_sh.destroy();
    clouds_geom.destroy();
}
