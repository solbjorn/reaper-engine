#include "stdafx.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wdeprecated-copy-with-dtor");
XR_DIAG_IGNORE("-Wfloat-equal");

#include <absl/random/random.h>

XR_DIAG_POP();

#include <concurrentqueue.h>

namespace xr
{
namespace
{
class random
{
private:
    moodycamel::ConcurrentQueue<std::unique_ptr<absl::BitGen>> q;

public:
    [[nodiscard]] std::unique_ptr<absl::BitGen> get()
    {
        std::unique_ptr<absl::BitGen> gen;

        if (!q.try_dequeue(gen))
            gen = std::make_unique<absl::BitGen>();

        return gen;
    }

    void put(std::unique_ptr<absl::BitGen>&& gen) { q.enqueue(std::move(gen)); }
};

random rnd;
} // namespace

s32 random_s32(s32 min, s32 max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    s32 ret = absl::Uniform<s32>(absl::IntervalClosed, *gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}

s32 random_s32_below(s32 min, s32 max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    s32 ret = absl::Uniform<s32>(*gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}

s64 random_s64(s64 min, s64 max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    s64 ret = absl::Uniform<s64>(absl::IntervalClosed, *gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}

u32 random_u32(u32 min, u32 max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    u32 ret = absl::Uniform<u32>(absl::IntervalClosed, *gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}

u32 random_u32_below(u32 min, u32 max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    u32 ret = absl::Uniform<u32>(*gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}

u64 random_u64(u64 min, u64 max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    u64 ret = absl::Uniform<u64>(absl::IntervalClosed, *gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}

float random_float(float min, float max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    float ret = absl::Uniform<float>(absl::IntervalClosed, *gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}

double random_double_below(double min, double max)
{
    std::unique_ptr<absl::BitGen> gen = rnd.get();

    double ret = absl::Uniform<double>(*gen, min, max);
    rnd.put(std::move(gen));

    return ret;
}
} // namespace xr
