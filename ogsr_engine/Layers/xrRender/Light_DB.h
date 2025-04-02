#pragma once

#include "light.h"
#include "light_package.h"

class CLight_DB
{
private:
    xr_vector<ref_light> v_static;
    xr_vector<ref_light> v_hemi;

public:
    ref_light sun{};
    ref_light rain{};
    light_Package package;

    void add_light(light* L);

    void Load(IReader* fs);
    void LoadHemi();
    void Unload();

    light* Create();
    void Update();
};
