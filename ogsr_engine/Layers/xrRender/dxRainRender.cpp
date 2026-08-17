#include "stdafx.h"

#include "dxRainRender.h"

#include "../../xr_3da/IGame_Persistent.h"
#include "../../xr_3da/perlin.h"
#include "../../xr_3da/Rain.h"

namespace
{
// Warning: duplicated in rain.cpp
constexpr float max_desired_items = 2500;
constexpr float source_radius = 15; // 12.5f;
constexpr float source_offset = 20.f; // 40
constexpr float max_distance = source_offset * 1.5f; // 1.25f;
constexpr float sink_offset = -(max_distance - source_offset);
constexpr float drop_length = 5.f;
constexpr float drop_width = 0.30f;
constexpr float drop_max_angle = deg2rad(35.f); // 10
constexpr float particles_time = .3f;
} // namespace

dxRainRender::dxRainRender()
{
    RainPerlin = std::make_unique<CPerlinNoise1D>();
    RainPerlin->SetOctaves(2);
    RainPerlin->SetAmplitude(0.66666f);

    static constexpr gsl::czstring path{"dm\\rain.dm"};
    const auto F = XR_ASSERT_VAL(absl::WrapUnique(FS.r_open("$game_meshes$", path)), "rain model not found", path);
    DM_Drop = ::RImplementation.model_CreateDM(F.get());

    SH_Rain.create("effects\\rain", "fx\\fx_rain");

    hGeom_Rain.create(FVF::F_LIT, SGeometry::default_vb(), RImplementation.QuadIB);
    XR_ASSERT(hGeom_Rain.stride() == sizeof(FVF::LIT));

    hGeom_Drops.create(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, SGeometry::default_vb(), SGeometry::default_ib());
    XR_ASSERT(hGeom_Drops.stride() == sizeof(IRender_DetailModel::fvfVertexOut));

    if (RImplementation.o.ssfx_rain)
        SH_Splash.create("effects\\rain_splash", "fx\\fx_rain");
}

dxRainRender::~dxRainRender() { ::RImplementation.model_Delete(DM_Drop); }

void dxRainRender::Render(CEffect_Rain& owner)
{
    const float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor < EPS_L)
        return;

    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    float _drop_len = drop_length;
    float _drop_width = drop_width;
    ref_shader& _splash_SH = DM_Drop->shader;
    constexpr const char* s_shader_setup = "ssfx_rain_setup";

    // SSS Rain shader is available
    if (RImplementation.o.ssfx_rain)
    {
        _drop_len = ps_ssfx_rain_1.x;
        _drop_width = ps_ssfx_rain_1.y;
        _splash_SH = SH_Splash;
    }

    // visual
    const float factor_visual = factor / 2.f + .5f;
    const Fvector3& f_rain_color = g_pGamePersistent->Environment().CurrentEnv->rain_color;
    const u32 u_rain_color = color_rgba_f(f_rain_color.x, f_rain_color.y, f_rain_color.z, factor_visual);

    if (!owner.items.empty())
    {
        // perform update
        const Fvector& vEye = Device.vCameraPosition;

        const auto verts = cmd_list.Vertex.Lock<FVF::LIT>(owner.items.size() * 4);
        std::size_t written{0};

        for (const auto& one : owner.items)
        {
            // Build line
            const Fvector& pos_head = one.P;
            Fvector pos_trail;
            pos_trail.mad(pos_head, one.D, -_drop_len * factor_visual);

            // Culling
            Fvector sC, lineD;
            float sR;
            sC.sub(pos_head, pos_trail);
            lineD.normalize(sC);
            sC.mul(.5f);
            sR = sC.magnitude();
            sC.add(pos_trail);
            if (!RImplementation.ViewBase.testSphere_dirty(sC, sR))
                continue;

            static constexpr Fvector2 UV[2][4]{{Fvector2{0.0f, 1.0f}, Fvector2{0.0f, 0.0f}, Fvector2{1.0f, 1.0f}, Fvector2{1.0f, 0.0f}},
                                               {Fvector2{1.0f, 0.0f}, Fvector2{1.0f, 1.0f}, Fvector2{0.0f, 0.0f}, Fvector2{0.0f, 1.0f}}};

            // Everything OK - build vertices
            Fvector P, lineTop, camDir;
            camDir.sub(sC, vEye);
            camDir.normalize();
            lineTop.crossproduct(camDir, lineD);
            float w = _drop_width;
            u32 s = one.uv_set;

            const auto v = verts.subspan(written, 4);
            P.mad(pos_trail, lineTop, -w);
            v[0].set(P, u_rain_color, UV[s][0].x, UV[s][0].y);
            P.mad(pos_trail, lineTop, w);
            v[1].set(P, u_rain_color, UV[s][1].x, UV[s][1].y);
            P.mad(pos_head, lineTop, -w);
            v[2].set(P, u_rain_color, UV[s][2].x, UV[s][2].y);
            P.mad(pos_head, lineTop, w);
            v[3].set(P, u_rain_color, UV[s][3].x, UV[s][3].y);
            written += 4;
        }

        const auto vOffset = cmd_list.Vertex.Unlock<FVF::LIT>(written);

        // Render if needed
        if (written == 0)
            goto drops;

        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_xform_world(Fidentity);
        cmd_list.set_Shader(SH_Rain);
        cmd_list.set_Geometry(hGeom_Rain);
        cmd_list.Render(D3DPT_TRIANGLELIST, vOffset, 0, written, 0, written / 2);

        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_c(s_shader_setup, ps_ssfx_rain_2); // Alpha, Brigthness, Refraction, Reflection
    }

drops:
    // Particles
    CEffect_Rain::Particle* P = owner.particle_active;
    if (!P)
        return;

    float dt = Device.fTimeDelta;

    cmd_list.set_Shader(_splash_SH);
    cmd_list.set_c(s_shader_setup, ps_ssfx_rain_3); // Alpha, Refraction

    const auto vstep = DM_Drop->vertices.size();
    const auto istep = DM_Drop->indices.size();
    const auto particles_cache =
        std::min(cmd_list.Vertex.GetSize() / (vstep * sizeof(IRender_DetailModel::fvfVertexOut)), cmd_list.Index.GetSize() / (istep * sizeof(u16)));
    const auto vCount_Lock = particles_cache * vstep;
    const auto iCount_Lock = particles_cache * istep;

    auto verts = cmd_list.Vertex.Lock<IRender_DetailModel::fvfVertexOut>(vCount_Lock);
    auto indices = cmd_list.Index.Lock(iCount_Lock);
    std::size_t vwritten{0};
    std::size_t iwritten{0};

    while (P != nullptr)
    {
        CEffect_Rain::Particle* next = P->next;

        // Update
        // P can be zero sometimes and it crashes
        P->time -= dt;
        if (P->time < 0)
        {
            owner.p_free(P);
            P = next;
            continue;
        }

        // Render
        if (!RImplementation.ViewBase.testSphere_dirty(P->bounds.P, P->bounds.R))
        {
            P = next;
            continue;
        }

        // Build matrix
        float scale = P->time / particles_time;
        Fmatrix mScale;
        mScale.scale(scale, scale, scale);
        Fmatrix mXform;
        mXform.mul_43(P->mXForm, mScale);

        // XForm verts
        DM_Drop->transfer(mXform, verts.subspan(vwritten, vstep), u_rain_color, indices.subspan(iwritten, istep), vwritten);
        vwritten += vstep;
        iwritten += istep;

        if (vwritten == vCount_Lock || iwritten == iCount_Lock)
        {
            // flush
            const auto v_offset = cmd_list.Vertex.Unlock<IRender_DetailModel::fvfVertexOut>(vCount_Lock);
            const auto i_offset = cmd_list.Index.Unlock(iCount_Lock);

            cmd_list.set_Geometry(hGeom_Drops);
            cmd_list.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, iCount_Lock / 3);

            verts = cmd_list.Vertex.Lock<IRender_DetailModel::fvfVertexOut>(vCount_Lock);
            indices = cmd_list.Index.Lock(iCount_Lock);

            vwritten = 0;
            iwritten = 0;
        }

        P = next;
    }

    // Flush if needed
    const auto v_offset = cmd_list.Vertex.Unlock<IRender_DetailModel::fvfVertexOut>(vwritten);
    const auto i_offset = cmd_list.Index.Unlock(iwritten);

    if (vwritten > 0 || iwritten > 0)
    {
        cmd_list.set_Geometry(hGeom_Drops);
        cmd_list.Render(D3DPT_TRIANGLELIST, v_offset, 0, vwritten, i_offset, iwritten / 3);
    }
}

void dxRainRender::Calculate(CEffect_Rain& owner)
{
    float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor < EPS_L)
        return;

    XR_TRACY_ZONE_SCOPED();

    // Prepare correct angle and distance to hit the player
    Fvector Rain_Axis{0.f, -1.f, 0.f};
    Fvector2 Rain_Offset;

    const float Wind_Direction = -g_pGamePersistent->Environment().CurrentEnv->wind_direction;

    // Wind Velocity [ From 0 ~ 1000 to 0 ~ 1 ]
    float Wind_Velocity = g_pGamePersistent->Environment().CurrentEnv->wind_velocity * 0.001f;
    clamp(Wind_Velocity, 0.0f, 1.0f);

    auto Prepare = [&] {
        // Wind direction

        // Wind gust, to add variation.
        const float Wind_Gust = RainPerlin->GetContinious(Device.fTimeGlobal * 0.3f) * 2.0f;

        // Wind velocity [ 0 ~ 1 ]
        float _Wind_Velocity = Wind_Velocity + Wind_Gust;

        clamp(_Wind_Velocity, 0.0f, 1.0f);

        // Wind velocity controles the angle
        const float pitch = drop_max_angle * _Wind_Velocity;
        Rain_Axis.setHP(Wind_Direction, pitch - PI_DIV_2);

        // Get distance
        float dist = _sin(pitch) * source_offset;
        const float C = PI_DIV_2 - pitch;
        dist /= _sin(C);

        // 0 is North
        const float fixNorth = Wind_Direction - PI_DIV_2;

        // Set offset
        Rain_Offset.set(dist * _cos(fixNorth), dist * _sin(fixNorth));
    };

    Prepare();

    float _drop_speed = 1.0f;
    float rain_max_particles = max_desired_items;
    float rain_radius = source_radius;

    // SSS Rain shader is available
    if (RImplementation.o.ssfx_rain)
    {
        _drop_speed = ps_ssfx_rain_1.z;
        rain_max_particles = ps_ssfx_rain_drops_setup.x;
        rain_radius = ps_ssfx_rain_drops_setup.y;
    }

    const size_t desired_items = iFloor(0.01f * (1.f + factor * 99.0f) * rain_max_particles);

    if (size_t sz = owner.items.size(); sz < desired_items)
    {
        // born _new_ if needed
        for (; sz < desired_items; sz++)
        {
            CEffect_Rain::Item one;
            owner.Born(one, rain_radius, _drop_speed, Wind_Velocity, Rain_Offset, Rain_Axis);
            owner.items.emplace_back(std::move(one));
        }
    }
    else
    {
        owner.items.resize(desired_items);
    }

    // build source plane
    float b_radius_wrap_sqr = _sqr((rain_radius * 1.5f));
    Fplane src_plane;
    constexpr Fvector norm{0.0f, -1.0f, 0.0f};
    Fvector upper;
    upper.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);
    src_plane.build(upper, norm);

    const Fvector& vEye = Device.vCameraPosition;

    for (auto& one : owner.items)
    {
        if (one.dwTime_Hit < Device.dwTimeGlobal)
        {
            owner.Hit(one.Phit);
        }

        if (one.dwTime_Life < Device.dwTimeGlobal)
        {
            owner.Born(one, rain_radius, _drop_speed, Wind_Velocity, Rain_Offset, Rain_Axis);
        }

        float dt = Device.fTimeDelta;
        one.P.mad(one.D, one.fSpeed * dt);

        Device.Statistic->TEST1.Begin();
        Fvector wdir;
        wdir.set(one.P.x - vEye.x, 0, one.P.z - vEye.z);
        float wlen = wdir.square_magnitude();
        if (wlen > b_radius_wrap_sqr)
        {
            wlen = _sqrt(wlen);

            if ((one.P.y - vEye.y) < sink_offset)
            {
                // need born
                one.invalidate();
            }
            else
            {
                Fvector inv_dir, src_p;
                inv_dir.invert(one.D);
                wdir.div(wlen);
                one.P.mad(one.P, wdir, -(wlen + rain_radius));
                if (src_plane.intersectRayPoint(one.P, inv_dir, src_p))
                {
                    float dist_sqr = one.P.distance_to_sqr(src_p);
                    float height = max_distance;

                    if (owner.RayPick(src_p, one.D, height, collide::rqtBoth))
                    {
                        if (_sqr(height) <= dist_sqr)
                        {
                            one.invalidate(); // need born
                        }
                        else
                        {
                            owner.RenewItem(one, height - _sqrt(dist_sqr), TRUE); // fly to point
                        }
                    }
                    else
                    {
                        owner.RenewItem(one, max_distance - _sqrt(dist_sqr), FALSE); // fly ...
                    }
                }
                else
                {
                    // need born
                    one.invalidate();
                }
            }
        }
        Device.Statistic->TEST1.End();
    }
}

const Fsphere& dxRainRender::GetDropBounds() const { return DM_Drop->bv_sphere; }
