////////////////////////////////////////////////////////////////////////////
//	Module 		: script_render_device_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script render device script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_render_device.h"

#include "../xr_3da/x_ray.h"

namespace
{
[[nodiscard]] bool is_device_paused(const CRenderDevice*) { return !!Device.Paused(); }
void set_device_paused(const CRenderDevice*, bool b) { Device.Pause(b, TRUE, FALSE, "set_device_paused_script"); }

[[nodiscard]] bool is_app_ready() { return !!g_appLoaded; }

[[nodiscard]] u32 time_global(const CRenderDevice* self)
{
    THROW(self);
    return self->dwTimeGlobal;
}
} // namespace

template <>
void CScriptRenderDevice::script_register(sol::state_view& lua)
{
    lua.new_usertype<CRenderDevice>(
        "render_device", sol::no_constructor, "width", sol::readonly(&CRenderDevice::dwWidth), "height", sol::readonly(&CRenderDevice::dwHeight), "time_delta",
        sol::readonly(&CRenderDevice::dwTimeDelta), "f_time_delta", sol::readonly(&CRenderDevice::fTimeDelta), "cam_pos", sol::readonly(&CRenderDevice::vCameraPosition), "cam_dir",
        sol::readonly(&CRenderDevice::vCameraDirection), "cam_top", sol::readonly(&CRenderDevice::vCameraTop), "cam_right", sol::readonly(&CRenderDevice::vCameraRight), "fov",
        sol::readonly(&CRenderDevice::fFOV), "aspect_ratio", sol::readonly(&CRenderDevice::fASPECT), "time_global", &time_global, "precache_frame",
        sol::readonly(&CRenderDevice::dwPrecacheFrame), "frame", sol::readonly(&CRenderDevice::dwFrame), "is_paused", &is_device_paused, "pause", &set_device_paused);

    lua.set_function("app_ready", &is_app_ready);
}
