#pragma once

#include "../xrRender/ColorMapManager.h"

class light;
class light_Package;

//	no less than 2
#define VOLUMETRIC_SLICES 100

class CRenderTarget : public IRender_Target
{
private:
    u32 dwWidth[R__NUM_CONTEXTS];
    u32 dwHeight[R__NUM_CONTEXTS];
    u32 dwAccumulatorClearMark;

public:
    u32 dwLightMarkerID;

    IBlender* b_accum_point;
    IBlender* b_accum_spot;

    IBlender* b_accum_spot_msaa;
    IBlender* b_accum_point_msaa;

#ifdef DEBUG
    struct dbg_line_t
    {
        Fvector P0, P1;
        u32 color;
    };
    xr_vector<std::pair<Fsphere, Fcolor>> dbg_spheres;
    xr_vector<dbg_line_t> dbg_lines;
    xr_vector<Fplane> dbg_planes;
#endif

    // Base targets
    xr_vector<ref_rt> rt_Base;
    ref_rt rt_Base_Depth;

    // MRT-path
    ref_rt rt_MSAADepth; // z-buffer for MSAA deferred shading
    ref_rt rt_Generic_0_r; // MRT generic 0
    ref_rt rt_Generic_1_r; // MRT generic 1
    ref_rt rt_Generic;
    ref_rt rt_Position; // 64bit,	fat	(x,y,z,?)				(eye-space)
    ref_rt rt_Color; // 64/32bit,fat	(r,g,b,specular-gloss)	(or decompressed MET-8-8-8-8)

    //
    ref_rt rt_Accumulator; // 64bit		(r,g,b,specular)
    ref_rt rt_sunshafts_0; // ss0
    ref_rt rt_sunshafts_1; // ss1
    ref_rt rt_SunShaftsMask;
    ref_rt rt_SunShaftsMaskSmoothed;
    ref_rt rt_SunShaftsPass0;
    ref_rt rt_Generic_0; // 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
    ref_rt rt_Generic_1; // 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.

    resptr_core<CRT, resptrcode_crt> rt_Generic_temp;

    ref_rt rt_secondVP; // 32bit		(r,g,b,a) --//#SM+#-- +SecondVP+

    ref_rt rt_dof;
    ref_rt rt_BeforeUi;

    ref_rt rt_blur_h_2;
    ref_rt rt_blur_2;
    ref_rt rt_blur_2_zb;

    ref_rt rt_blur_h_4;
    ref_rt rt_blur_4;
    ref_rt rt_blur_4_zb;

    ref_rt rt_blur_h_8;
    ref_rt rt_blur_8;
    ref_rt rt_blur_8_zb;

    ref_rt rt_smaa_edgetex;
    ref_rt rt_smaa_blendtex;

    //	Igor: for volumetric lights
    ref_rt rt_Generic_2; // 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
    ref_rt rt_Bloom_1; // 32bit, dim/4	(r,g,b,?)
    ref_rt rt_Bloom_2; // 32bit, dim/4	(r,g,b,?)
    ref_rt rt_LUM_64; // 64bit, 64x64,	log-average in all components
    ref_rt rt_LUM_8; // 64bit, 8x8,		log-average in all components

    ref_rt rt_LUM_pool[CHWCaps::MAX_GPUS * 2]; // 1xfp32,1x1,		exp-result -> scaler
    ref_texture t_LUM_src; // source
    ref_texture t_LUM_dest; // destination & usage for current frame

    // smap
    ref_rt rt_smap_depth; // 24(32) bit,	depth

    // Screen Space Shaders Stuff
    ref_rt rt_ssfx;
    ref_rt rt_ssfx_temp;
    ref_rt rt_ssfx_temp2;
    ref_rt rt_ssfx_temp3;

    ref_rt rt_ssfx_accum;
    ref_rt rt_ssfx_hud;
    ref_rt rt_ssfx_ssr;
    ref_rt rt_ssfx_water;
    ref_rt rt_ssfx_water_waves;
    ref_rt rt_ssfx_ao;
    ref_rt rt_ssfx_il;

    ref_rt rt_ssfx_sss;
    ref_rt rt_ssfx_sss_ext;
    ref_rt rt_ssfx_sss_ext2;
    ref_rt rt_ssfx_sss_tmp;
    ref_rt rt_ssfx_bloom1;
    ref_rt rt_ssfx_bloom_emissive;
    ref_rt rt_ssfx_bloom_lens;
    ref_rt rt_ssfx_rain;
    ref_rt rt_ssfx_volumetric;
    ref_rt rt_ssfx_volumetric_tmp;

    ref_rt rt_ssfx_bloom_tmp2;
    ref_rt rt_ssfx_bloom_tmp4;
    ref_rt rt_ssfx_bloom_tmp8;
    ref_rt rt_ssfx_bloom_tmp16;
    ref_rt rt_ssfx_bloom_tmp32;
    ref_rt rt_ssfx_bloom_tmp64;

    ref_rt rt_ssfx_bloom_tmp32_2;
    ref_rt rt_ssfx_bloom_tmp16_2;
    ref_rt rt_ssfx_bloom_tmp8_2;
    ref_rt rt_ssfx_bloom_tmp4_2;

    ref_rt rt_ssfx_prevPos;

    ref_shader s_ssfx_water;
    ref_shader s_ssfx_water_blur;
    ref_shader s_ssfx_water_ssr;
    ref_shader s_ssfx_ao;
    ref_shader s_ssfx_hud[5];

    Fmatrix Matrix_previous, Matrix_current;
    Fmatrix Matrix_HUD_previous;
    Fvector3 Position_previous;

    // Textures
    ref_texture t_noise[TEX_jitter_count];

private:
    // OCCq
    ref_shader s_occq;
    ref_shader s_ssss_ogse;
    ref_shader s_ssss_mrmnwar;

    // Accum
    ref_shader s_accum_mask;
    ref_shader s_accum_direct;
    ref_shader s_accum_direct_volumetric;
    ref_shader s_accum_point;
    ref_shader s_accum_spot;
    ref_shader s_accum_volume;
    ref_shader s_blur;
    ref_shader s_dof;
    ref_shader s_gasmask_drops;
    ref_shader s_gasmask_dudv;
    ref_shader s_pp_antialiasing;

    //	DX10 Rain
    ref_shader s_rain;

    ref_shader s_rain_msaa;
    ref_shader s_accum_direct_volumetric_msaa;
    ref_shader s_accum_mask_msaa;
    ref_shader s_accum_direct_msaa;
    ref_shader s_mark_msaa_edges;
    ref_shader s_accum_point_msaa;
    ref_shader s_accum_spot_msaa;
    ref_shader s_accum_volume_msaa;

    // Screen Space Shaders Stuff
    ref_shader s_ssfx_rain;
    ref_shader s_ssfx_bloom;
    ref_shader s_ssfx_bloom_lens;
    ref_shader s_ssfx_bloom_upsample;
    ref_shader s_ssfx_bloom_downsample;
    ref_shader s_ssfx_sss_ext;
    ref_shader s_ssfx_sss;

    ref_shader s_ssfx_ssr;
    ref_shader s_ssfx_volumetric_blur;

    ref_geom g_accum_point;
    ref_geom g_accum_spot;
    ref_geom g_accum_omnipart;
    ref_geom g_accum_volumetric;

    ID3DVertexBuffer* g_accum_point_vb;
    ID3DIndexBuffer* g_accum_point_ib;

    ID3DVertexBuffer* g_accum_omnip_vb;
    ID3DIndexBuffer* g_accum_omnip_ib;

    ID3DVertexBuffer* g_accum_spot_vb;
    ID3DIndexBuffer* g_accum_spot_ib;

    ID3DVertexBuffer* g_accum_volumetric_vb;
    ID3DIndexBuffer* g_accum_volumetric_ib;

    // Bloom
    ref_geom g_bloom_build;
    ref_geom g_bloom_filter;
    ref_shader s_bloom;
    ref_shader s_bloom_msaa;
    float f_bloom_factor;

    // Luminance
    ref_shader s_luminance;
    float f_luminance_adapt;

    // Combine
    ref_geom g_combine;
    ref_geom g_combine_VP; // xy=p,zw=tc
    ref_geom g_combine_2UV;
    ref_geom g_combine_cuboid;
    ref_geom g_aa_blur;
    ref_geom g_aa_AA;
    ref_shader s_combine;
    ref_shader s_combine_msaa;
    ref_shader s_combine_volumetric;

public:
    ref_shader s_postprocess;
    ref_shader s_postprocess_msaa;
    ref_geom g_postprocess;
    ref_shader s_menu;
    ref_geom g_menu;

private:
    float im_noise_time;
    u32 im_noise_shift_w;
    u32 im_noise_shift_h;

    float param_blur;
    float param_gray;
    float param_duality_h;
    float param_duality_v;
    float param_noise;
    float param_noise_scale;
    float param_noise_fps;
    u32 param_color_base;
    u32 param_color_gray;
    Fvector param_color_add;

    //	Color mapping
    float param_color_map_influence;
    float param_color_map_interpolate;
    ColorMapManager color_map_manager;

    //	Igor: used for volumetric lights
    bool m_bHasActiveVolumetric;
    bool m_bHasActiveVolumetric_spot;

public:
    CRenderTarget();
    ~CRenderTarget();
    void accum_point_geom_create();
    void accum_point_geom_destroy();
    void accum_omnip_geom_create();
    void accum_omnip_geom_destroy();
    void accum_spot_geom_create();
    void accum_spot_geom_destroy();
    //	Igor: used for volumetric lights
    void accum_volumetric_geom_create();
    void accum_volumetric_geom_destroy();

    ID3DRenderTargetView* get_base_rt() { return rt_Base[HW.CurrentBackBuffer]->pRT; }
    ID3DDepthStencilView* get_base_zb() { return rt_Base_Depth->pZRT[R__IMM_CTX_ID]; }

    void u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, ID3DDepthStencilView* zb);
    void u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, const ref_rt& _zb)
    {
        u_setrt(cmd_list, _1, _2, _3, _zb ? _zb->pZRT[cmd_list.context_id] : nullptr);
    }
    void u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, ID3DDepthStencilView* zb);
    void u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _zb) { u_setrt(cmd_list, _1, _2, _zb ? _zb->pZRT[cmd_list.context_id] : nullptr); }
    void u_setrt(CBackend& cmd_list, u32 W, u32 H, ID3DRenderTargetView* _1, ID3DRenderTargetView* _2, ID3DRenderTargetView* _3, ID3DDepthStencilView* zb);
    void u_setrt(CBackend& cmd_list, u32 W, u32 H, ID3DRenderTargetView* _1, ID3DRenderTargetView* _2, ID3DRenderTargetView* _3, const ref_rt& _zb)
    {
        u_setrt(cmd_list, W, H, _1, _2, _3, _zb ? _zb->pZRT[cmd_list.context_id] : nullptr);
    }

    void u_compute_texgen_screen(CBackend& cmd_list, Fmatrix& dest);
    void u_compute_texgen_jitter(CBackend& cmd_list, Fmatrix& dest);
    void u_calc_tc_noise(Fvector2& p0, Fvector2& p1);
    void u_calc_tc_duality_ss(Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1);
    BOOL u_need_PP();
    bool u_need_CM();

    void PhaseSSSS();
    void phase_blur();
    void phase_dof();
    void phase_gasmask_drops();
    void phase_gasmask_dudv();
    void ProcessSMAA();
    void phase_scene_prepare();
    void phase_scene_begin();
    inline void phase_scene_end() { disable_aniso(); }
    void phase_occq();
    void phase_downsamp();
    void phase_wallmarks();
    void phase_smap_direct(CBackend& cmd_list, const light* L, u32 sub_phase);
    void phase_smap_spot_clear(CBackend& cmd_list);
    void phase_smap_spot(CBackend& cmd_list, const light* L);
    void phase_accumulator(CBackend& cmd_list);
    void phase_vol_accumulator(CBackend& cmd_list);
    void shadow_direct(light* L, u32 dls_phase);

    // SSS Stuff
    void phase_ssfx_rain(); // Bloom PP
    void phase_ssfx_bloom(); // Bloom PP
    void phase_ssfx_sss(); // SSS
    void phase_ssfx_sss_ext(light_Package& LP); // SSS Spot lights

    void phase_ssfx_ssr(); // SSR Phase
    void phase_ssfx_volumetric_blur(); // Volumetric Blur
    void phase_ssfx_water_blur(); // Water Blur
    void phase_ssfx_water_waves(); // Water Waves
    void phase_ssfx_ao(); // AO
    void phase_ssfx_il(); // IL

    void phase_rain();
    void draw_rain(const light& RainSetup);

    void mark_msaa_edges();

    bool need_to_render_sunshafts();

    BOOL enable_scissor(light* L); // true if intersects near plane
    void disable_aniso();

    void draw_volume(const light* L);
    void accum_direct_cascade(CBackend& cmd_list, u32 sub_phase, const Fmatrix& xform, const Fmatrix& xform_prev, float fBias);
    void accum_direct_blend();
    void accum_direct_volumetric(CBackend& cmd_list, u32 sub_phase, const u32 Offset, const Fmatrix& mShadow);
    void accum_point(light* L);
    void accum_spot(light* L);
    //	Igor: for volumetric lights
    void accum_volumetric(light* L);
    void phase_bloom();
    void phase_luminance();
    void phase_combine();
    void phase_combine_volumetric();
    void phase_pp();

    u32 get_width(const CBackend& cmd_list) const { return dwWidth[cmd_list.context_id]; }
    u32 get_height(const CBackend& cmd_list) const { return dwHeight[cmd_list.context_id]; }

    u32 get_width(ctx_id_t context_id) override { return dwWidth[context_id]; }
    u32 get_height(ctx_id_t context_id) override { return dwHeight[context_id]; }

    void set_blur(float f) { param_blur = f; }
    void set_gray(float f) { param_gray = f; }
    void set_duality_h(float f) { param_duality_h = _abs(f); }
    void set_duality_v(float f) { param_duality_v = _abs(f); }
    void set_noise(float f) { param_noise = f; }
    void set_noise_scale(float f) { param_noise_scale = f; }
    void set_noise_fps(float f) { param_noise_fps = _abs(f) + EPS_S; }
    void set_color_base(u32 f) { param_color_base = f; }
    void set_color_gray(u32 f) { param_color_gray = f; }
    void set_color_add(const Fvector& f) { param_color_add = f; }
    void set_cm_imfluence(float f) { param_color_map_influence = f; }
    void set_cm_interpolate(float f) { param_color_map_interpolate = f; }
    void set_cm_textures(const shared_str& tex0, const shared_str& tex1) { color_map_manager.SetTextures(tex0, tex1); }

    //	Need to reset stencil only when marker overflows.
    //	Don't clear when render for the first time
    void reset_light_marker(CBackend& cmd_list, bool bResetStencil = false);
    void increment_light_marker(CBackend& cmd_list);

#ifdef DEBUG
    IC void dbg_addline(Fvector& P0, Fvector& P1, u32 c) { dbg_lines.emplace_back(dbg_line_t{P0, P1, c}); }
    IC void dbg_addplane(Fplane& P0, u32 c) { dbg_planes.push_back(P0); }
#else
    IC void dbg_addline(Fvector& P0, Fvector& P1, u32 c) {}
    IC void dbg_addplane(Fplane& P0, u32 c) {}
#endif

private:
    void RenderScreenQuad(const u32 w, const u32 h, ID3DRenderTargetView* rt, ref_selement& sh, string_unordered_map<const char*, Fvector4*>* consts = nullptr);
    void RenderScreenQuad(const u32 w, const u32 h, ref_rt& rt, ref_selement& sh, string_unordered_map<const char*, Fvector4*>* consts = nullptr);
};
