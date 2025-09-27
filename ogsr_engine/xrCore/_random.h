#ifndef __XR_RANDOM_H
#define __XR_RANDOM_H

namespace xr
{
s32 random_s32(s32 min, s32 max);
s32 random_s32_below(s32 min, s32 max);
s64 random_s64(s64 min, s64 max);

u32 random_u32(u32 min, u32 max);
u32 random_u32_below(u32 min, u32 max);
u64 random_u64(u64 min, u64 max);

float random_float(float min, float max);
double random_double_below(double min, double max);

inline s32 random_s32(s32 max) { return random_s32(0, max); }
inline s32 random_s32_below(s32 max) { return random_s32_below(0, max); }
inline u32 random_u32(u32 max) { return random_u32(0u, max); }
inline u32 random_u32_below(u32 max) { return random_u32_below(0u, max); }
inline float random_float(float max) { return random_float(0.f, max); }
} // namespace xr

class CRandom final
{
public:
    [[nodiscard]] inline s32 randI(const s32 max) { return xr::random_s32_below(max); }
    [[nodiscard]] inline s32 randI(const s32 min, const s32 max) { return xr::random_s32_below(min, max); }

    [[nodiscard]] inline float randF() { return xr::random_float(1.f); }
    [[nodiscard]] inline float randF(const float max) { return xr::random_float(max); }
    [[nodiscard]] inline float randF(const float min, const float max) { return xr::random_float(min, max); }
    [[nodiscard]] inline float randFs(const float range) { return xr::random_float(-range, range); }
    [[nodiscard]] inline float randFs(const float range, const float offs) { return xr::random_float(offs - range, offs + range); }
};

inline CRandom Random;

inline s32 rand() { return xr::random_s32(std::numeric_limits<s32>::max()); }

#endif /* __XR_RANDOM_H */
