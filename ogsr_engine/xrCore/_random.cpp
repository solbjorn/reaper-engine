#include "stdafx.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wdeprecated-copy-with-dtor");
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wsign-conversion");

#include <absl/random/random.h>

XR_DIAG_POP();

#include <bitmap_object_pool.hpp>

namespace xr
{
namespace
{
tzcnt_utils::BitmapObjectPool<absl::BitGen> rnd;
}

s32 random_s32(s32 min, s32 max) { return absl::Uniform<s32>(absl::IntervalClosed, rnd.acquire_scoped().value, min, max); }
s32 random_s32_below(s32 min, s32 max) { return absl::Uniform<s32>(rnd.acquire_scoped().value, min, max); }
s64 random_s64(s64 min, s64 max) { return absl::Uniform<s64>(absl::IntervalClosed, rnd.acquire_scoped().value, min, max); }

u32 random_u32(u32 min, u32 max) { return absl::Uniform<u32>(absl::IntervalClosed, rnd.acquire_scoped().value, min, max); }
u32 random_u32_below(u32 min, u32 max) { return absl::Uniform<u32>(rnd.acquire_scoped().value, min, max); }
u64 random_u64(u64 min, u64 max) { return absl::Uniform<u64>(absl::IntervalClosed, rnd.acquire_scoped().value, min, max); }

f32 random_f32(f32 min, f32 max) { return absl::Uniform<f32>(absl::IntervalClosed, rnd.acquire_scoped().value, min, max); }
f64 random_f64_below(f64 min, f64 max) { return absl::Uniform<f64>(rnd.acquire_scoped().value, min, max); }
} // namespace xr
