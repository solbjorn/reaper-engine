#include "StdAfx.h"
#include "light.h"

constexpr float RSQRTDIV2 = 0.70710678118654752440084436210485f;

light::light(void) : ISpatial(g_SpatialSpace)
{
    spatial.type = STYPE_LIGHTSOURCE;
    flags.type = POINT;
    flags.bStatic = false;
    flags.bActive = false;
    flags.bShadow = false;
    flags.bVolumetric = false;
    flags.bHudMode = false;
    position.set(0, -1000, 0);
    direction.set(0, -1, 0);
    right.set(0, 0, 0);
    range = 8.f;
    cone = deg2rad(60.f);
    color.set(1, 1, 1, 1);

    m_volumetric_quality = 1;
    m_volumetric_intensity = 1;
    m_volumetric_distance = 1;

    frame_render = 0;
    virtual_size = MIN_VIRTUAL_SIZE;
    omnipart_num = 0;
    sss_id = -1;
    sss_refresh = 0;

    ZeroMemory(omnipart, sizeof(omnipart));
    s_spot = nullptr;
    s_point = nullptr;
    vis.frame2test = 0; // xffffffff;
    vis.query_id = 0;
    vis.query_order = 0;
    vis.visible = true;
    vis.pending = false;

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
        svis[id].id = id;
}

light::~light()
{
    for (auto& f : omnipart)
        xr_delete(f);

    set_active(false);

    // remove from Lights_LastFrame
    for (auto& p_light : RImplementation.Lights_LastFrame)
    {
        if (this == p_light)
        {
            for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
                p_light->svis[id].resetoccq();

            p_light = nullptr;
        }
    }

    if (vis.pending)
        RImplementation.occq_free(vis.query_id);
}

void light::set_texture(LPCSTR name)
{
    if ((nullptr == name) || (0 == name[0]))
    {
        // default shaders
        s_spot.destroy();
        s_point.destroy();
        return;
    }

#pragma todo("Only shadowed spot implements projective texture")
    string_path temp;

    s_spot.create(RImplementation.Target->b_accum_spot, xr_strconcat(temp, "r2\\accum_spot_", name), name);
    s_point.create(RImplementation.Target->b_accum_point, xr_strconcat(temp, "r2\\accum_point_", name), name);

    if (RImplementation.o.dx10_msaa)
    {
        s_spot_msaa.create(RImplementation.Target->b_accum_spot_msaa, xr_strconcat(temp, "r2\\accum_spot_", name), name);
        s_point_msaa.create(RImplementation.Target->b_accum_point_msaa, xr_strconcat(temp, "r2\\accum_point_", name), name);
    }
}

void light::set_active(bool a)
{
    if (a)
    {
        if (flags.bActive)
            return;
        flags.bActive = true;
        spatial_register();
        spatial_move();
        // Msg								("!!! L-register: %X",u32(this));

#ifdef DEBUG
        constexpr Fvector zero = {0, -1000, 0};
        if (position.similar(zero))
        {
            Msg("- Uninitialized light position.");
        }
#endif // DEBUG
    }
    else
    {
        if (!flags.bActive)
            return;
        flags.bActive = false;
        spatial_move();
        spatial_unregister();
        // Msg								("!!! L-unregister: %X",u32(this));
    }
}

void light::set_position(const Fvector& P)
{
    constexpr float eps = EPS_L; //_max	(range*0.001f,EPS_L);
    if (position.similar(P, eps))
        return;
    position.set(P);
    spatial_move();
}

void light::set_range(float R)
{
    const float eps = std::max(range * 0.1f, EPS_L);
    if (fsimilar(range, R, eps))
        return;
    range = R;
    spatial_move();
};

void light::set_cone(float angle)
{
    if (fsimilar(cone, angle))
        return;
    VERIFY(cone < deg2rad(121.f)); // 120 is hard limit for lights
    cone = angle;
    spatial_move();
}

void light::set_rotation(const Fvector& D, const Fvector& R)
{
    const Fvector old_D = direction;
    direction.normalize(D);
    right.normalize(R);
    if (!fsimilar(1.f, old_D.dotproduct(D)))
        spatial_move();
}

void light::spatial_move()
{
    switch (flags.type)
    {
    case IRender_Light::POINT: spatial.sphere.set(position, range); break;
    case IRender_Light::SPOT: {
        // minimal enclosing sphere around cone
        VERIFY2(cone < deg2rad(121.f), "Too large light-cone angle. Maybe you have passed it in 'degrees'?");
        if (cone >= PI_DIV_2)
        {
            // obtused-angled
            spatial.sphere.P.mad(position, direction, range);
            spatial.sphere.R = range * tanf(cone / 2.f);
        }
        else
        {
            // acute-angled
            spatial.sphere.R = range / (2.f * _sqr(_cos(cone / 2.f)));
            spatial.sphere.P.mad(position, direction, spatial.sphere.R);
        }
    }
    break;
    case IRender_Light::OMNIPART: {
        // is it optimal? seems to be...
        // spatial.sphere.P.mad		(position,direction,range);
        // spatial.sphere.R			= range;
        // This is optimal.
        const float fSphereR = range * RSQRTDIV2;
        spatial.sphere.P.mad(position, direction, fSphereR);
        spatial.sphere.R = fSphereR;
    }
    break;
    }

    // update spatial DB
    ISpatial::spatial_move();

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
        svis[id].invalidate();
}

vis_data& light::get_homdata()
{
    // commit vis-data
    hom.sphere.set(spatial.sphere.P, spatial.sphere.R);
    hom.box.set(spatial.sphere.P, spatial.sphere.P);
    hom.box.grow(spatial.sphere.R);
    return hom;
};

Fvector light::spatial_sector_point() { return position; }

//////////////////////////////////////////////////////////////////////////
// Xforms
void light::xform_calc()
{
    if (Device.dwFrame == m_xform_frame)
        return;
    m_xform_frame = Device.dwFrame;

    // build final rotation / translation
    Fvector L_dir, L_up, L_right;

    // dir
    L_dir.set(direction);
    float l_dir_m = L_dir.magnitude();
    if (_valid(l_dir_m) && l_dir_m > EPS_S)
        L_dir.div(l_dir_m);
    else
        L_dir.set(0, 0, 1);

    // R&N
    if (right.square_magnitude() > EPS)
    {
        // use specified 'up' and 'right', just enshure ortho-normalization
        L_right.set(right);
        L_right.normalize();
        L_up.crossproduct(L_dir, L_right);
        L_up.normalize();
        L_right.crossproduct(L_up, L_dir);
        L_right.normalize();
    }
    else
    {
        // auto find 'up' and 'right' vectors
        L_up.set(0, 1, 0);
        if (_abs(L_up.dotproduct(L_dir)) > .99f)
            L_up.set(0, 0, 1);
        L_right.crossproduct(L_up, L_dir);
        L_right.normalize();
        L_up.crossproduct(L_dir, L_right);
        L_up.normalize();
    }

    // matrix
    Fmatrix mR;
    mR.vm[0].set(L_right, 0);
    mR.vm[1].set(L_up, 0);
    mR.vm[2].set(L_dir, 0);
    mR.vm[3].set(position, 1);

    // switch
    switch (flags.type)
    {
    case IRender_Light::POINT: {
        // scale of identity sphere
        float L_R = range;
        Fmatrix mScale;
        mScale.scale(L_R, L_R, L_R);
        m_xform.mul_43(mR, mScale);
    }
    break;
    case IRender_Light::SPOT: {
        // scale to account range and angle
        float s = 2.f * range * tanf(cone / 2.f);
        Fmatrix mScale;
        mScale.scale(s, s, range); // make range and radius
        m_xform.mul_43(mR, mScale);
    }
    break;
    case IRender_Light::OMNIPART: {
        float L_R = 2 * range; // volume is half-radius
        Fmatrix mScale;
        mScale.scale(L_R, L_R, L_R);
        m_xform.mul_43(mR, mScale);
    }
    break;
    default: m_xform.identity(); break;
    }
}

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
static constexpr Fvector cmNorm[6] = {{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}};
static constexpr Fvector cmDir[6] = {{1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, -1.f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 0.f, -1.f}};

void light::export_to(light_Package& package)
{
    if (flags.bShadow)
    {
        switch (flags.type)
        {
        case IRender_Light::POINT: {
            // tough: create/update 6 shadowed lights
            if (nullptr == omnipart[0])
                for (auto& p_light : omnipart)
                    p_light = xr_new<light>();
            for (int f = 0; f < 6; f++)
            {
                light* L = omnipart[f];
                Fvector R;
                R.crossproduct(cmNorm[f], cmDir[f]);
                L->set_type(IRender_Light::OMNIPART);
                L->set_shadow(true);
                L->set_position(position);
                L->set_rotation(cmDir[f], R);
                L->set_cone(PI_DIV_2);
                L->set_range(range);
                L->set_color(color);
                L->spatial.sector_id = spatial.sector_id;
                L->s_spot = s_spot;
                L->s_point = s_point;
                L->set_virtual_size(virtual_size); // Set virtual size
                L->omnipart_num = f;
                L->omipart_parent = omnipart[0];
                L->flags.bActive = true;

                // Holger - do we need to export msaa stuff as well ?
                if (RImplementation.o.dx10_msaa)
                {
                    L->s_point_msaa = s_point_msaa;
                    L->s_spot_msaa = s_spot_msaa;
                }

                //	Igor: add volumetric support
                if (flags.bVolumetricManual || ps_ssfx_volumetric.x <= 0)
                    L->set_volumetric(flags.bVolumetric, flags.bVolumetricManual);

                L->set_volumetric_quality(m_volumetric_quality);
                L->set_volumetric_intensity(m_volumetric_intensity);
                L->set_volumetric_distance(m_volumetric_distance);

                package.v_shadowed.push_back(L);
            }
        }
        break;
        case IRender_Light::SPOT:
            this->set_volumetric_intensity(m_volumetric_intensity);
            package.v_shadowed.push_back(this);
            break;
        }
    }
    else
    {
        switch (flags.type)
        {
        case IRender_Light::POINT: package.v_point.push_back(this); break;
        case IRender_Light::SPOT: package.v_spot.push_back(this); break;
        }
    }
}

void light::set_attenuation_params(float a0, float a1, float a2, float fo)
{
    attenuation0 = a0;
    attenuation1 = a1;
    attenuation2 = a2;
    falloff = fo;
}

extern float r_ssaGLOD_start, r_ssaGLOD_end;
extern float ps_r2_slight_fade;

float light::get_LOD()
{
    if (!flags.bShadow)
        return 1;
    const float distSQ = Device.vCameraPosition.distance_to_sqr(spatial.sphere.P) + EPS;
    distance = distSQ;
    const float ssa = ps_r2_slight_fade * spatial.sphere.R / distSQ;
    const float lod = _sqrt(clampr((ssa - r_ssaGLOD_end) / (r_ssaGLOD_start - r_ssaGLOD_end), 0.f, 1.f));
    return lod;
}
