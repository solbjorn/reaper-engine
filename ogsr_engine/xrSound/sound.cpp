#include "stdafx.h"

#include "SoundRender_CoreA.h"

xr_token* snd_devices_token = nullptr;
u32 snd_device_id = u32(-1);

namespace
{
CSoundRender_CoreA* impl{};
}

void CSound_manager_interface::_create(int stage)
{
    if (stage == 0)
    {
        impl = xr_new<CSoundRender_CoreA>();
        Sound = SoundRender = impl;

        if (strstr(Core.Params, "-nosound"))
        {
            SoundRender->bPresent = FALSE;
            return;
        }

        SoundRender->bPresent = TRUE; // make assumption that sound present
    }

    if (!SoundRender->bPresent)
        return;

    SoundRender->_initialize(stage);
}

void CSound_manager_interface::_destroy()
{
    SoundRender->_clear();
    SoundRender = nullptr;

    Sound = nullptr;

    xr_delete(impl);
}
