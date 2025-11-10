#ifndef __XR_RANDOM_H
#define __XR_RANDOM_H

namespace xr
{
[[nodiscard]] s32 random_s32(s32 min, s32 max);
[[nodiscard]] s32 random_s32_below(s32 min, s32 max);
[[nodiscard]] s64 random_s64(s64 min, s64 max);

[[nodiscard]] u32 random_u32(u32 min, u32 max);
[[nodiscard]] u32 random_u32_below(u32 min, u32 max);
[[nodiscard]] u64 random_u64(u64 min, u64 max);

[[nodiscard]] f32 random_f32(f32 min, f32 max);
[[nodiscard]] f64 random_f64_below(f64 min, f64 max);

[[nodiscard]] inline s32 random_s32(s32 max) { return random_s32(0, max); }
[[nodiscard]] inline s32 random_s32_below(s32 max) { return random_s32_below(0, max); }
[[nodiscard]] inline u32 random_u32(u32 max) { return random_u32(0u, max); }
[[nodiscard]] inline u32 random_u32_below(u32 max) { return random_u32_below(0u, max); }
[[nodiscard]] inline f32 random_f32(f32 max) { return random_f32(0.0f, max); }
} // namespace xr

class CRandom final
{
public:
    [[nodiscard]] static inline s32 randI(s32 max) { return xr::random_s32_below(max); }
    [[nodiscard]] static inline s32 randI(s32 min, s32 max) { return xr::random_s32_below(min, max); }

    [[nodiscard]] static inline f32 randF() { return xr::random_f32(1.0f); }
    [[nodiscard]] static inline f32 randF(f32 max) { return xr::random_f32(max); }
    [[nodiscard]] static inline f32 randF(f32 min, f32 max) { return xr::random_f32(min, max); }
    [[nodiscard]] static inline f32 randFs(f32 range) { return xr::random_f32(-range, range); }
    [[nodiscard]] static inline f32 randFs(f32 range, f32 offs) { return xr::random_f32(offs - range, offs + range); }
};

inline CRandom Random;

[[nodiscard]] inline s32 rand() { return xr::random_s32(std::numeric_limits<s32>::max()); }

#endif /* __XR_RANDOM_H */
