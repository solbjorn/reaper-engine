#include "stdafx.h"

#include <absl/random/random.h>
#include <concurrentqueue.h>

class xr_random
{
private:
    moodycamel::ConcurrentQueue<absl::BitGen*> q;

public:
    absl::BitGen* get()
    {
        absl::BitGen* gen;

        if (!q.try_dequeue(gen))
            gen = xr_new<absl::BitGen>();

        return gen;
    }

    bool put(absl::BitGen* gen) { return q.enqueue(gen); }

    xr_random() = default;

    ~xr_random()
    {
        absl::BitGen* gen;

        while (q.try_dequeue(gen))
            xr_delete(gen);
    }
};

static xr_random rnd;

u32 get_random_u32(u32 min, u32 max)
{
    absl::BitGen* gen = rnd.get();

    u32 ret = absl::Uniform<u32>(absl::IntervalClosed, *gen, min, max);
    rnd.put(gen);

    return ret;
}

u32 get_random_u32_below(u32 min, u32 max)
{
    absl::BitGen* gen = rnd.get();

    u32 ret = absl::Uniform<u32>(*gen, min, max);
    rnd.put(gen);

    return ret;
}

int get_random_int(int min, int max)
{
    absl::BitGen* gen = rnd.get();

    int ret = absl::Uniform<int>(absl::IntervalClosed, *gen, min, max);
    rnd.put(gen);

    return ret;
}

int get_random_int_below(int min, int max)
{
    absl::BitGen* gen = rnd.get();

    int ret = absl::Uniform<int>(*gen, min, max);
    rnd.put(gen);

    return ret;
}

float get_random_float(float min, float max)
{
    absl::BitGen* gen = rnd.get();

    float ret = absl::Uniform<float>(absl::IntervalClosed, *gen, min, max);
    rnd.put(gen);

    return ret;
}

u64 get_random_u64(u64 min, u64 max)
{
    absl::BitGen* gen = rnd.get();

    u64 ret = absl::Uniform<u64>(absl::IntervalClosed, *gen, min, max);
    rnd.put(gen);

    return ret;
}

s64 get_random_s64(s64 min, s64 max)
{
    absl::BitGen* gen = rnd.get();

    s64 ret = absl::Uniform<s64>(absl::IntervalClosed, *gen, min, max);
    rnd.put(gen);

    return ret;
}

double get_random_double_below(double min, double max)
{
    absl::BitGen* gen = rnd.get();

    double ret = absl::Uniform<double>(*gen, min, max);
    rnd.put(gen);

    return ret;
}
