#include "stdafx.h"

#include "../../xrCDB/frustum.h"

#include "../xrRenderDX10/StateManager/dx10StateManager.h"
#include "../xrRenderDX10/StateManager/dx10ShaderResourceStateCache.h"

void CBackend::OnFrameEnd()
{
    context()->ClearState();
    Invalidate();
}

void CBackend::OnFrameBegin()
{
    PGO(Msg("PGO:*****frame[%d]*****", Device.dwFrame));

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

    for (auto [pc, vc, gc, hc, dc, cc] : std::views::zip(m_aPixelConstants, m_aVertexConstants, m_aGeometryConstants, m_aHullConstants, m_aDomainConstants, m_aComputeConstants))
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

    for (u32 gs_it = 0; gs_it < CTexture::mtMaxGeometryShaderTextures;)
        textures_gs[gs_it++] = nullptr;
    for (u32 hs_it = 0; hs_it < CTexture::mtMaxHullShaderTextures;)
        textures_hs[hs_it++] = nullptr;
    for (u32 ds_it = 0; ds_it < CTexture::mtMaxDomainShaderTextures;)
        textures_ds[ds_it++] = nullptr;
    for (u32 cs_it = 0; cs_it < CTexture::mtMaxComputeShaderTextures;)
        textures_cs[cs_it++] = nullptr;

    context_id = R__IMM_CTX_ID;

    for (u32 ps_it = 0; ps_it < CTexture::mtMaxPixelShaderTextures;)
        textures_ps[ps_it++] = nullptr;
    for (u32 vs_it = 0; vs_it < CTexture::mtMaxVertexShaderTextures;)
        textures_vs[vs_it++] = nullptr;
}

void CBackend::set_ClipPlanes(u32, Fplane*, u32)
{
    // TODO: DX10: Implement in the corresponding vertex shaders
    // Use this to set up location, were shader setup code will get data
    // VERIFY(!"CBackend::set_ClipPlanes not implemented!");
    return;
}

void CBackend::set_ClipPlanes(u32 _enable, Fmatrix* _xform, u32 fmask)
{
    if (!_enable)
    {
        // TODO: DX10: Implement in the corresponding vertex shaders
        // Use this to set up location, were shader setup code will get data
        // VERIFY(!"CBackend::set_ClipPlanes not implemented!");
        return;
    }

    if (HW.Caps.geometry.dwClipPlanes == 0)
        return;

    VERIFY(_xform && fmask);

    CFrustum F;
    F.CreateFromMatrix(*_xform, fmask);
    set_ClipPlanes(_enable, F.planes, F.p_count);
}

void CBackend::set_Textures(STextureList* textures_list)
{
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

    auto it = textures_list->begin();
    const auto end = textures_list->end();

    for (; it != end; ++it)
    {
        std::pair<u32, ref_texture>& loader = *it;
        gsl::index load_id{loader.first};
        CTexture* load_surf = loader.second._get();

        if (load_id < CTexture::rstVertex)
        {
            // Set up pixel shader resources
            VERIFY(load_id < CTexture::mtMaxPixelShaderTextures);
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
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                    load_surf->last_slice = load_surf->curr_slice;
                }
            }
        }
        else if (load_id < CTexture::rstGeometry)
        {
            // Set up pixel shader resources
            VERIFY(load_id < std::to_underlying(CTexture::rstVertex) + std::to_underlying(CTexture::mtMaxVertexShaderTextures));

            // vertex only //d-map or vertex
            gsl::index load_id_remapped{load_id - CTexture::rstVertex};
            if (load_id_remapped > _last_vs)
                _last_vs = load_id_remapped;
            if (textures_vs[load_id_remapped] != load_surf)
            {
                textures_vs[load_id_remapped] = load_surf;
#ifdef DEBUG
                stat.textures++;
#endif
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstHull)
        {
            // Set up pixel shader resources
            VERIFY(load_id < std::to_underlying(CTexture::rstGeometry) + std::to_underlying(CTexture::mtMaxGeometryShaderTextures));

            // vertex only //d-map or vertex
            gsl::index load_id_remapped{load_id - CTexture::rstGeometry};
            if (load_id_remapped > _last_gs)
                _last_gs = load_id_remapped;
            if (textures_gs[load_id_remapped] != load_surf)
            {
                textures_gs[load_id_remapped] = load_surf;
#ifdef DEBUG
                stat.textures++;
#endif
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstDomain)
        {
            // Set up pixel shader resources
            VERIFY(load_id < std::to_underlying(CTexture::rstHull) + std::to_underlying(CTexture::mtMaxHullShaderTextures));

            // vertex only //d-map or vertex
            gsl::index load_id_remapped{load_id - CTexture::rstHull};
            if (load_id_remapped > _last_hs)
                _last_hs = load_id_remapped;
            if (textures_hs[load_id_remapped] != load_surf)
            {
                textures_hs[load_id_remapped] = load_surf;
#ifdef DEBUG
                stat.textures++;
#endif
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstCompute)
        {
            // Set up pixel shader resources
            VERIFY(load_id < std::to_underlying(CTexture::rstDomain) + std::to_underlying(CTexture::mtMaxDomainShaderTextures));

            // vertex only //d-map or vertex
            gsl::index load_id_remapped{load_id - CTexture::rstDomain};
            if (load_id_remapped > _last_ds)
                _last_ds = load_id_remapped;
            if (textures_ds[load_id_remapped] != load_surf)
            {
                textures_ds[load_id_remapped] = load_surf;
#ifdef DEBUG
                stat.textures++;
#endif
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstInvalid)
        {
            // Set up pixel shader resources
            VERIFY(load_id < std::to_underlying(CTexture::rstCompute) + std::to_underlying(CTexture::mtMaxComputeShaderTextures));

            // vertex only //d-map or vertex
            gsl::index load_id_remapped{load_id - CTexture::rstCompute};
            if (load_id_remapped > _last_cs)
                _last_cs = load_id_remapped;
            if (textures_cs[load_id_remapped] != load_surf)
            {
                textures_cs[load_id_remapped] = load_surf;
#ifdef DEBUG
                stat.textures++;
#endif
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else
        {
            VERIFY(load_id >= CTexture::rstInvalid, "Invalid enum");
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
        // HW.pDevice->PSSetShaderResources(_last_ps, 1, &pRes);
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
        // HW.pDevice->VSSetShaderResources(_last_vs, 1, &pRes);
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
        // HW.pDevice->GSSetShaderResources(_last_gs, 1, &pRes);
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
    CROS_impl& LT = *(CROS_impl*)O.renderable_ROS();
    LT.update_smooth(&O);

    xr_memcpy32(&o, &LT.get_lmaterial());
    o.hemi *= 0.75f;
    o.sun *= 0.75f;
}

void CBackend::apply_lmaterial(IRenderable* O)
{
    if (O && O->renderable_ROS())
        apply_object(*O);

    R_constant* C = get_c("s_base")._get(); // get sampler
    if (!C)
        return;

    VERIFY(RC_dest_sampler == C->destination);
    VERIFY(RC_dx10texture == C->type);

    CTexture* T = get_ActiveTexture(u32(C->samp.index));
    VERIFY(T);

    float mtl = T ? T->m_material : 0.f;

    hemi.set_material(o.hemi, o.sun, 0, (mtl < 5 ? (mtl + .5f) / 4.f : mtl));
    hemi.set_pos_faces(o.hemi_cube[CROS_impl::CUBE_FACE_POS_X], o.hemi_cube[CROS_impl::CUBE_FACE_POS_Y], o.hemi_cube[CROS_impl::CUBE_FACE_POS_Z]);
    hemi.set_neg_faces(o.hemi_cube[CROS_impl::CUBE_FACE_NEG_X], o.hemi_cube[CROS_impl::CUBE_FACE_NEG_Y], o.hemi_cube[CROS_impl::CUBE_FACE_NEG_Z]);
}
