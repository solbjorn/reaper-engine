#include "stdafx.h"
#include "render.h"

IRender_interface::~IRender_interface() {};

// IRender_interface*	Render		= NULL;

// resources
IRender_Light::~IRender_Light() { ::Render->light_destroy(this); }
IRender_Glow::~IRender_Glow() { ::Render->glow_destroy(this); }

ShExports shader_exports{};
GRASS_SHADER_DATA grass_shader_data{};
