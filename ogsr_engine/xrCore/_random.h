#ifndef __XRCORE_RANDOM_H
#define __XRCORE_RANDOM_H

u32 get_random_u32(u32 min, u32 max);
u32 get_random_u32_below(u32 min, u32 max);
int get_random_int(int min, int max);
int get_random_int_below(int min, int max);
float get_random_float(float min, float max);

u64 get_random_u64(u64 min, u64 max);
s64 get_random_s64(s64 min, s64 max);
double get_random_double_below(double min, double max);

inline u32 get_random_u32(u32 max) { return get_random_u32(0u, max); }
inline u32 get_random_u32_below(u32 max) { return get_random_u32_below(0u, max); }
inline int get_random_int(int max) { return get_random_int(0, max); }
inline int get_random_int_below(int max) { return get_random_int_below(0, max); }
inline float get_random_float(float max) { return get_random_float(0.f, max); }

class CRandom final
{
public:
    [[nodiscard]] inline s32 randI(const s32 max) { return get_random_int_below(max); }
    [[nodiscard]] inline s32 randI(const s32 min, const s32 max) { return get_random_int_below(min, max); }

    [[nodiscard]] inline float randF() { return get_random_float(1.f); }
    [[nodiscard]] inline float randF(const float max) { return get_random_float(max); }
    [[nodiscard]] inline float randF(const float min, const float max) { return get_random_float(min, max); }
    [[nodiscard]] inline float randFs(const float range) { return get_random_float(-range, range); }
    [[nodiscard]] inline float randFs(const float range, const float offs) { return get_random_float(offs - range, offs + range); }
};

inline CRandom Random;

inline int rand() { return get_random_int(RAND_MAX); }

#endif /* __XRCORE_RANDOM_H */
