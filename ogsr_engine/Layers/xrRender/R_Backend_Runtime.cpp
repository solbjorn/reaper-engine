#include "stdafx.h"

#include "../../xrCDB/Frustum.h"

#include "../xrRenderDX10/StateManager/dx10StateManager.h"
#include "../xrRenderDX10/StateManager/dx10ShaderResourceStateCache.h"

void CBackend::OnFrameEnd()
{
    context()->ClearState();
    Invalidate();
}

void CBackend::OnFrameBegin()
{
    Invalidate();
    // DX9 sets base rt nd base zb by default
    RImplementation.rmNormal(*this);
    set_RT(RImplementation.Target->get_base_rt());
    set_ZB(RImplementation.Target->rt_Base_Depth->pZRT[context_id]);
    memset(&stat, 0, sizeof(stat));
    set_Stencil(FALSE);
}

void CBackend::Invalidate()
{
    XR_TRACY_ZONE_SCOPED();

    pRT[0] = nullptr;
    pRT[1] = nullptr;
    pRT[2] = nullptr;
    pRT[3] = nullptr;
    pZB = nullptr;

    decl = nullptr;
    vb = nullptr;
    ib = nullptr;
    vb_stride = 0;

    state = nullptr;
    ps = nullptr;
    vs = nullptr;
    gs = nullptr;

    hs = nullptr;
    ds = nullptr;
    cs = nullptr;

    ctable = nullptr;
    T = nullptr;

    stencil_enable = std::numeric_limits<u32>::max();
    stencil_func = std::numeric_limits<u32>::max();
    stencil_ref = std::numeric_limits<u32>::max();
    stencil_mask = std::numeric_limits<u32>::max();
    stencil_writemask = std::numeric_limits<u32>::max();
    stencil_fail = std::numeric_limits<u32>::max();
    stencil_pass = std::numeric_limits<u32>::max();
    stencil_zfail = std::numeric_limits<u32>::max();
    cull_mode = std::numeric_limits<u32>::max();
    fill_mode = std::numeric_limits<u32>::max();
    z_enable = std::numeric_limits<u32>::max();
    z_func = std::numeric_limits<u32>::max();
    alpha_ref = std::numeric_limits<u32>::max();
    colorwrite_mask = std::numeric_limits<u32>::max();

    // Since constant buffers are unmapped (for DirecX 10)
    // transform setting handlers should be unmapped too.
    xforms.unmap();

    m_pInputLayout = nullptr;
    m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    m_bChangedRTorZB = false;
    m_pInputSignature = nullptr;

    for (auto [pc, vc, gc, hc, dc, cc] :
         std::views::zip(m_aPixelConstants, m_aVertexConstants, m_aGeometryConstants, m_aHullConstants, m_aDomainConstants, m_aComputeConstants))
    {
        pc._set(nullptr);
        vc._set(nullptr);
        gc._set(nullptr);
        hc._set(nullptr);
        dc._set(nullptr);
        cc._set(nullptr);
    }

    StateManager.Reset();
    // Redundant call. Just no note that we need to unmap const
    // if we create dedicated class.
    StateManager.UnmapConstants();
    SRVSManager.ResetDeviceState();

    std::memset(&textures_ps, 0, sizeof(textures_ps));
    std::memset(&textures_vs, 0, sizeof(textures_vs));
    std::memset(&textures_gs, 0, sizeof(textures_gs));
    std::memset(&textures_hs, 0, sizeof(textures_hs));
    std::memset(&textures_ds, 0, sizeof(textures_ds));
    std::memset(&textures_cs, 0, sizeof(textures_cs));

    context_id = R__IMM_CTX_ID;
}

void CBackend::set_Textures(STextureList* textures_list)
{
    XR_TRACY_ZONE_SCOPED();

    // TODO: expose T invalidation method
    // if (T == textures_list) // disabled due to cases when the set of resources the same, but different srv is need to be bind
    //    return;
    T = textures_list;

    // If resources weren't set at all we should clear from resource #0.
    gsl::index _last_ps{-1};
    gsl::index _last_vs{-1};
    gsl::index _last_gs{-1};
    gsl::index _last_hs{-1};
    gsl::index _last_ds{-1};
    gsl::index _last_cs{-1};

    for (auto& loader : *textures_list)
    {
        gsl::index load_id{loader.first};
        CTexture* load_surf = loader.second._get();

        if (load_id < CTexture::rstVertex)
        {
            // Set up pixel shader resources
            XR_ASSERT(load_id < CTexture::mtMaxPixelShaderTextures);

            // ordinary pixel surface
            if (load_id > _last_ps)
                _last_ps = load_id;

            if (textures_ps[load_id] != load_surf || (load_surf && (load_surf->last_slice != load_surf->curr_slice)))
            {
                textures_ps[load_id] = load_surf;

#ifdef DEBUG
                stat.textures++;
#endif

                if (load_surf)
                {
                    load_surf->bind(*this, load_id);
                    load_surf->last_slice = load_surf->curr_slice;
                }
            }
        }
        else if (load_id < CTexture::rstGeometry)
        {
            // Set up pixel shader resources
            // vertex only //d-map or vertex
            const gsl::index load_id_remapped = XR_ASSERT_VAL(load_id - CTexture::rstVertex < CTexture::mtMaxVertexShaderTextures);
            if (load_id_remapped > _last_vs)
                _last_vs = load_id_remapped;

            if (textures_vs[load_id_remapped] != load_surf)
            {
                textures_vs[load_id_remapped] = load_surf;

#ifdef DEBUG
                stat.textures++;
#endif

                if (load_surf)
                    load_surf->bind(*this, load_id);
            }
        }
        else if (load_id < CTexture::rstHull)
        {
            // Set up pixel shader resources
            // vertex only //d-map or vertex
            const gsl::index load_id_remapped = XR_ASSERT_VAL(load_id - CTexture::rstGeometry < CTexture::mtMaxGeometryShaderTextures);
            if (load_id_remapped > _last_gs)
                _last_gs = load_id_remapped;

            if (textures_gs[load_id_remapped] != load_surf)
            {
                textures_gs[load_id_remapped] = load_surf;

#ifdef DEBUG
                stat.textures++;
#endif

                if (load_surf)
                    load_surf->bind(*this, load_id);
            }
        }
        else if (load_id < CTexture::rstDomain)
        {
            // Set up pixel shader resources
            // vertex only //d-map or vertex
            const gsl::index load_id_remapped = XR_ASSERT_VAL(load_id - CTexture::rstHull < CTexture::mtMaxHullShaderTextures);
            if (load_id_remapped > _last_hs)
                _last_hs = load_id_remapped;

            if (textures_hs[load_id_remapped] != load_surf)
            {
                textures_hs[load_id_remapped] = load_surf;

#ifdef DEBUG
                stat.textures++;
#endif

                if (load_surf)
                    load_surf->bind(*this, load_id);
            }
        }
        else if (load_id < CTexture::rstCompute)
        {
            // Set up pixel shader resources
            // vertex only //d-map or vertex
            const gsl::index load_id_remapped = XR_ASSERT_VAL(load_id - CTexture::rstDomain < CTexture::mtMaxDomainShaderTextures);
            if (load_id_remapped > _last_ds)
                _last_ds = load_id_remapped;

            if (textures_ds[load_id_remapped] != load_surf)
            {
                textures_ds[load_id_remapped] = load_surf;

#ifdef DEBUG
                stat.textures++;
#endif

                if (load_surf)
                    load_surf->bind(*this, load_id);
            }
        }
        else if (load_id < CTexture::rstInvalid)
        {
            // Set up pixel shader resources
            // vertex only //d-map or vertex
            const gsl::index load_id_remapped = XR_ASSERT_VAL(load_id - CTexture::rstCompute < CTexture::mtMaxComputeShaderTextures);
            if (load_id_remapped > _last_cs)
                _last_cs = load_id_remapped;

            if (textures_cs[load_id_remapped] != load_surf)
            {
                textures_cs[load_id_remapped] = load_surf;

#ifdef DEBUG
                stat.textures++;
#endif

                if (load_surf)
                    load_surf->bind(*this, load_id);
            }
        }
        else
        {
            XR_PANIC("invalid texture type", load_id);
        }
    }

    // clear remaining stages (PS)
    for (++_last_ps; _last_ps < CTexture::mtMaxPixelShaderTextures; _last_ps++)
    {
        if (!textures_ps[_last_ps])
            continue;

        textures_ps[_last_ps] = nullptr;

        // TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes{};
        SRVSManager.SetPSResource(_last_ps, pRes);
    }

    // clear remaining stages (VS)
    for (++_last_vs; _last_vs < CTexture::mtMaxVertexShaderTextures; _last_vs++)
    {
        if (!textures_vs[_last_vs])
            continue;

        textures_vs[_last_vs] = nullptr;

        // TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes{};
        SRVSManager.SetVSResource(_last_vs, pRes);
    }

    // clear remaining stages (VS)
    for (++_last_gs; _last_gs < CTexture::mtMaxGeometryShaderTextures; _last_gs++)
    {
        if (!textures_gs[_last_gs])
            continue;

        textures_gs[_last_gs] = nullptr;

        // TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes{};
        SRVSManager.SetGSResource(_last_gs, pRes);
    }

    for (++_last_hs; _last_hs < CTexture::mtMaxHullShaderTextures; _last_hs++)
    {
        if (!textures_hs[_last_hs])
            continue;

        textures_hs[_last_hs] = nullptr;

        // TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes{};
        SRVSManager.SetHSResource(_last_hs, pRes);
    }

    for (++_last_ds; _last_ds < CTexture::mtMaxDomainShaderTextures; _last_ds++)
    {
        if (!textures_ds[_last_ds])
            continue;

        textures_ds[_last_ds] = nullptr;

        // TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes{};
        SRVSManager.SetDSResource(_last_ds, pRes);
    }

    for (++_last_cs; _last_cs < CTexture::mtMaxComputeShaderTextures; _last_cs++)
    {
        if (!textures_cs[_last_cs])
            continue;

        textures_cs[_last_cs] = nullptr;

        // TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes{};
        SRVSManager.SetCSResource(_last_cs, pRes);
    }
}

void CBackend::SetupStates()
{
    set_CullMode(CULL_CCW);

    SSManager.SetMaxAnisotropy(ps_r__tf_Anisotropic);
    SSManager.SetMipLODBias(ps_r__tf_Mipbias);
}

// Device dependance
void CBackend::OnDeviceCreate()
{
    context()->QueryInterface(IID_PPV_ARGS(&pAnnotation));

    // Debug Draw
    InitializeDebugDraw();

    // invalidate caching
    Invalidate();
}

void CBackend::OnDeviceDestroy()
{
    // Debug Draw
    DestroyDebugDraw();

    //  Destroy state managers
    StateManager.Reset();

    _RELEASE(pAnnotation);
}

void CBackend::apply_object(IRenderable& O)
{
    CROS_impl& LT = *smart_cast<CROS_impl*>(O.renderable_ROS());
    LT.update_smooth(&O);

    xr_memcpy32(&o, &LT.get_lmaterial());
    o.hemi *= 0.75f;
    o.sun *= 0.75f;
}

void CBackend::apply_lmaterial(IRenderable* O)
{
    XR_TRACY_ZONE_SCOPED();

    if (O && O->renderable_ROS())
        apply_object(*O);

    R_constant* C = get_c("s_base")._get(); // get sampler
    if (!C)
        return;

    XR_ASSERT(C->destination == RC_dest_sampler);
    XR_ASSERT(C->type == RC_dx10texture);

    CTexture* T = XR_ASSERT_VAL(get_ActiveTexture(C->samp.index) != nullptr);
    const auto mtl = T->m_material;

    hemi.set_material(o.hemi, o.sun, 0, (mtl < 5 ? (mtl + .5f) / 4.f : mtl));
    hemi.set_pos_faces(o.hemi_cube[CROS_impl::CUBE_FACE_POS_X], o.hemi_cube[CROS_impl::CUBE_FACE_POS_Y], o.hemi_cube[CROS_impl::CUBE_FACE_POS_Z]);
    hemi.set_neg_faces(o.hemi_cube[CROS_impl::CUBE_FACE_NEG_X], o.hemi_cube[CROS_impl::CUBE_FACE_NEG_Y], o.hemi_cube[CROS_impl::CUBE_FACE_NEG_Z]);
}
