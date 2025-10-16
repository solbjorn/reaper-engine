#pragma once

#include "Sound.h"

class CSoundRender_Core;
class CSoundRender_Source;
class CSoundRender_Emitter;
class CSoundRender_EmitterParams;
class CSoundRender_Target;
class CSoundRender_Environment;
class SoundEnvironment_LIB;

constexpr u32 sdef_target_count_submit = 4; // amount of buffers should be submitted to API
constexpr gsl::index sdef_target_count_prefill{10}; //
constexpr u32 sdef_target_block = 100; // ms
constexpr u32 sdef_env_version = 4; // current version of env-def
constexpr u32 sdef_level_version = 1; // current version of level-def
constexpr float s_f_def_event_pulse = 0.5f; // sec
