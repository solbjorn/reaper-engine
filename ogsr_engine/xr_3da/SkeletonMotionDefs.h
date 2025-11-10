#pragma once

constexpr inline u32 MAX_PARTS{4};

constexpr inline f32 SAMPLE_FPS{30.f};
constexpr inline f32 SAMPLE_SPF{1.0f / SAMPLE_FPS};
constexpr inline f32 END_EPS{SAMPLE_SPF + EPS};
constexpr inline f32 KEY_Quant{32767.f};
constexpr inline f32 KEY_QuantI{1.0f / KEY_Quant};
