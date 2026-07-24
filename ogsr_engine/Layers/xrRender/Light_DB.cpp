#include "StdAfx.h"

#include "../../xr_3da/_d3d_extensions.h"
#include "../../xr_3da/xrLevel.h"
#include "../../xr_3da/IGame_Persistent.h"
#include "../../xr_3da/Environment.h"

#include "R_light.h"
#include "Light_DB.h"

void CLight_DB::Load(IReader* fs)
{
    IReader* F = fs->open_chunk(fsL_LIGHT_DYNAMIC);

    // Light itself
    sun._set(nullptr);

    const size_t size = F->length();
    constexpr auto element = sizeof(Flight) + 4;
    const size_t count = size / element;
    XR_ASSERT(count * element == size, "", count, element);
    v_static.reserve(count);

    for (size_t i = 0; i < count; i++)
    {
        light* L = Create();
        L->flags.bStatic = true;
        Flight Ldata;

        F->advance(sizeof(u32));
        F->r(&Ldata, sizeof(Flight));

        if (Ldata.type == D3DLIGHT_DIRECTIONAL)
        {
            Fvector tmp_R;
            tmp_R.set(1, 0, 0);

            L->set_type(IRender_Light::DIRECT);
            L->set_shadow(true);
            L->set_rotation(Ldata.direction, tmp_R);

            sun._set(L);
        }
        else
        {
            Fvector tmp_D, tmp_R;
            tmp_D.set(0, 0, -1); // forward
            tmp_R.set(1, 0, 0); // right

            // point
            L->set_type(IRender_Light::POINT);
            L->set_position(Ldata.position);
            L->set_rotation(tmp_D, tmp_R);
            L->set_range(Ldata.range);
            L->set_color(Ldata.diffuse);
            L->set_shadow(true);
            L->set_active(true);

            v_static.emplace_back(L);
        }
    }

    F->close();

    XR_ASSERT(sun);
    rain._set(xr_new<light>());
}

void CLight_DB::LoadHemi()
{
    string_path fn_game;
    if (FS.exist(fn_game, "$level$", "build.lights"))
    {
        IReader* F = FS.r_open(fn_game);
        // Hemispheric light chunk
        IReader* chunk = F->open_chunk(fsL_HEADER);

        if (chunk)
        {
            const size_t size = chunk->length();
            constexpr auto element = sizeof(R_Light);
            const size_t count = size / element;

            if (const auto expected = count * element; size != expected)
                Msg("! {}: {} has {} garbage bytes (count: {}, expected: {}, real: {})", std::source_location::current().function_name(), fn_game,
                    size - expected, count, expected, size);

            v_hemi.reserve(count);

            for (size_t i = 0; i < count; i++)
            {
                R_Light Ldata;
                chunk->r(&Ldata, sizeof(R_Light));

                if (Ldata.type == D3DLIGHT_POINT)
                {
                    Fvector tmp_D, tmp_R;
                    tmp_D.set(0, 0, -1); // forward
                    tmp_R.set(1, 0, 0); // right

                    light* L = Create();
                    L->flags.bStatic = true;
                    L->set_type(IRender_Light::POINT);
                    L->set_position(Ldata.position);
                    L->set_rotation(tmp_D, tmp_R);
                    L->set_range(Ldata.range);
                    L->set_color(Ldata.diffuse.x, Ldata.diffuse.y, Ldata.diffuse.z);
                    L->set_active(true);
                    L->spatial.type = STYPE_LIGHTSOURCEHEMI;
                    L->set_attenuation_params(Ldata.attenuation0, Ldata.attenuation1, Ldata.attenuation2, Ldata.falloff);

                    v_hemi.emplace_back(L);
                }
            }

            chunk->close();
        }

        FS.r_close(F);
    }
}

void CLight_DB::Unload()
{
    v_static.clear();
    v_hemi.clear();

    sun.destroy();
    rain.destroy();
}

light* CLight_DB::Create()
{
    light* L = xr_new<light>();
    L->flags.bStatic = false;
    L->flags.bActive = false;
    L->flags.bShadow = true;
    return L;
}

void CLight_DB::add_light(light* L)
{
    if (Device.dwFrame == L->frame_render)
        return;
    L->frame_render = Device.dwFrame;
    if (L->flags.bStatic)
        return;
    L->export_to(package);
}

void CLight_DB::Update()
{
    // set sun params
    if (sun)
    {
        const CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        XR_DEBUG_ASSERT(_valid(E.sun_dir));

        // true sunlight direction
        Fvector dir, pos;
        dir.set(E.sun_dir).normalize();
        pos.mad(Device.vCameraPosition, dir, -500.f);

        sun->set_rotation(dir, smart_cast<light*>(sun._get())->right);
        sun->set_position(pos);
        sun->set_color(E.sun_color.x * ps_r2_sun_lumscale, E.sun_color.y * ps_r2_sun_lumscale, E.sun_color.z * ps_r2_sun_lumscale);
        sun->set_range(600.f);
    }

    // Clear selection
    package.clear();
}
