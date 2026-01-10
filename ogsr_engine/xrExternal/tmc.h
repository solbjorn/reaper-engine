#ifndef __XREXTERNAL_TMC_H
#define __XREXTERNAL_TMC_H

#define TMC_PRIORITY_COUNT 3
// #define TMC_TRIVIAL_TASK
#define TMC_USE_HWLOC

#ifndef TMC_IMPL
#include <tmc/all_headers.hpp>
#endif

namespace xr
{
namespace detail
{
enum class tmc_priority : int32_t
{
    high = 0,
    any,
    low,
    count,
};
static_assert(tmc_priority::count == tmc_priority{TMC_PRIORITY_COUNT});

#ifndef TMC_IMPL
inline tmc::ex_cpu_st tmc_g_ex_cpu_st;
#endif
} // namespace detail

constexpr inline size_t tmc_priority_high{std::to_underlying(xr::detail::tmc_priority::high)};
constexpr inline size_t tmc_priority_any{std::to_underlying(xr::detail::tmc_priority::any)};
constexpr inline size_t tmc_priority_low{std::to_underlying(xr::detail::tmc_priority::low)};

#ifndef TMC_IMPL
using tmc_atomic_wait_t = tmc::detail::atomic_wait_t;

[[nodiscard]] constexpr tmc::ex_cpu_st& tmc_cpu_st_executor() { return xr::detail::tmc_g_ex_cpu_st; }
#endif
} // namespace xr

#endif // !__XREXTERNAL_TMC_H
