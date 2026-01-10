#include "stdafx.h"

#include "xrcpuid.h"

#include <intrin.h>

namespace
{
ICF void xr_cpuid(u32* regs, u32 leaf)
{
#ifdef __clang__
    __cpuid(leaf, regs[0], regs[1], regs[2], regs[3]);
#else
    __cpuid(regs, leaf);
#endif
}

template <typename T, std::unsigned_integral In>
constexpr void from_unsigned(T& set, In in)
{
    for (size_t pos{}; pos < std::min(set.size(), sizeof(In) * 8); ++pos)
    {
        if (in & (In{1} << pos))
            set.set(pos);
    }
}
} // namespace

_processor_info::_processor_info()
{
    u32 cpinfo[4];
    // detect cpu vendor
    xr_cpuid(cpinfo, 0);
    memcpy(vendor, &(cpinfo[1]), sizeof(int));
    memcpy(vendor + sizeof(int), &(cpinfo[3]), sizeof(int));
    memcpy(vendor + 2 * sizeof(int), &(cpinfo[2]), sizeof(int));

    // detect cpu model
    xr_cpuid(cpinfo, 0x80000002);
    memcpy(brand, cpinfo, sizeof(cpinfo));
    xr_cpuid(cpinfo, 0x80000003);
    memcpy(brand + sizeof(cpinfo), cpinfo, sizeof(cpinfo));
    xr_cpuid(cpinfo, 0x80000004);
    memcpy(brand + 2 * sizeof(cpinfo), cpinfo, sizeof(cpinfo));

    // detect cpu main features
    xr_cpuid(cpinfo, 1);
    stepping = cpinfo[0] & 0xf;
    model = ((cpinfo[0] >> 4) & 0xf) | (((cpinfo[0] >> 16) & 0xf) << 4);
    family = ((cpinfo[0] >> 8) & 0xf) | (((cpinfo[0] >> 20) & 0xff) << 4);
    from_unsigned(m_f1_ECX, cpinfo[2]);
    from_unsigned(m_f1_EDX, cpinfo[3]);

    // and check 3DNow! support
    xr_cpuid(cpinfo, 0x80000001);
    from_unsigned(m_f81_ECX, cpinfo[2]);
    from_unsigned(m_f81_EDX, cpinfo[3]);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    m_dwNumberOfProcessors = sysInfo.dwNumberOfProcessors;
    fUsage = std::make_unique<float[]>(m_dwNumberOfProcessors);
    m_idleTime = std::make_unique<LARGE_INTEGER[]>(m_dwNumberOfProcessors);
    perfomanceInfo = std::make_unique<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[]>(m_dwNumberOfProcessors);
}

void _processor_info::print_topology()
{
    Msg("* NUMA nodes: %zu", topo.numa_count());

    const bool hybrid = topo.is_hybrid();
    Msg("* Hybrid architecture: %s", hybrid ? "yes" : "no");
    Msg("* Physical cores: %zu", topo.core_count());

    if (hybrid)
    {
        Msg("*  Performance cores: %zu", topo.cpu_kind_counts[0]);
        Msg("*  Efficiency cores: %zu", topo.cpu_kind_counts[1]);
    }

    Msg("* Logical processors: %zu", topo.pu_count());
    Msg("*  Container CPU quota: %g", topo.container_cpu_quota);
    Msg("* Core groups: %zu", topo.group_count());

    std::array<char, 256> out;

    for (const auto& group : topo.groups)
    {
        Msg("*  Group %zu: NUMA %zu, kind: %s, SMT: %zu", group.index, group.numa_index, group.cpu_kind == tmc::topology::cpu_kind::PERFORMANCE ? "performance" : "efficiency",
            group.smt_level);

        gsl::zstring pos = out.data() + xr_sprintf(out.data(), out.size(), "*   Cores: %zu", group.core_indexes[0]);

        for (auto idx : group.core_indexes | std::views::drop(1))
            pos += xr_sprintf(pos, out.size() - gsl::narrow_cast<size_t>(pos - out.data()), ", %zu", idx);

        Log(out.data());
    }

    Msg("* TMC threads: %zu (main) + %zu (ST)", tmc::cpu_executor().thread_count(), xr::tmc_cpu_st_executor().thread_count());

    if (!hybrid)
        return;

    xr_vector<size_t> perf, eff;

    for (const auto& thread : threads)
    {
        if (thread.group.cpu_kind == tmc::topology::cpu_kind::PERFORMANCE)
            perf.emplace_back(thread.index);
        else
            eff.emplace_back(thread.index);
    }

    gsl::zstring pos = out.data() + xr_sprintf(out.data(), out.size(), "*  Performance: %zu", perf[0]);

    for (auto idx : perf | std::views::drop(1))
        pos += xr_sprintf(pos, out.size() - gsl::narrow_cast<size_t>(pos - out.data()), ", %zu", idx);

    Log(out.data());

    pos = out.data() + xr_sprintf(out.data(), out.size(), "*  Efficiency: %zu", eff[0]);

    for (auto idx : eff | std::views::drop(1))
        pos += xr_sprintf(pos, out.size() - gsl::narrow_cast<size_t>(pos - out.data()), ", %zu", idx);

    Log(out.data());
}

namespace
{
[[nodiscard]] constexpr s64 SubtractTimes(FILETIME one, FILETIME two)
{
    ULARGE_INTEGER a, b;
    a.LowPart = one.dwLowDateTime;
    a.HighPart = one.dwHighDateTime;

    b.LowPart = two.dwLowDateTime;
    b.HighPart = two.dwHighDateTime;

    return gsl::narrow_cast<s64>(a.QuadPart) - gsl::narrow_cast<s64>(b.QuadPart);
}
} // namespace

bool _processor_info::getCPULoad(f64& val)
{
    FILETIME sysIdle, sysKernel, sysUser;
    // sysKernel include IdleTime
    if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0) // GetSystemTimes func FAILED return value is zero;
        return false;

    if (prevSysIdle.dwLowDateTime != 0 && prevSysIdle.dwHighDateTime != 0)
    {
        s64 sysIdleDiff, sysKernelDiff, sysUserDiff;
        sysIdleDiff = SubtractTimes(sysIdle, prevSysIdle);
        sysKernelDiff = SubtractTimes(sysKernel, prevSysKernel);
        sysUserDiff = SubtractTimes(sysUser, prevSysUser);

        s64 sysTotal = sysKernelDiff + sysUserDiff;
        s64 kernelTotal = sysKernelDiff - sysIdleDiff; // kernelTime - IdleTime = kernelTime, because sysKernel include IdleTime

        if (sysTotal > 0) // sometimes kernelTime > idleTime
            val = gsl::narrow_cast<f64>(kernelTotal + sysUserDiff) * 100.0 / gsl::narrow_cast<f64>(sysTotal);
    }

    prevSysIdle = sysIdle;
    prevSysKernel = sysKernel;
    prevSysUser = sysUser;

    return true;
}

void _processor_info::MTCPULoad()
{
    using NTQUERYSYSTEMINFORMATION = NTSTATUS(NTAPI*)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
    static const auto m_pNtQuerySystemInformation = reinterpret_cast<NTQUERYSYSTEMINFORMATION>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation"));

    if (!NT_SUCCESS(m_pNtQuerySystemInformation(SystemProcessorPerformanceInformation, perfomanceInfo.get(),
                                                sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * m_dwNumberOfProcessors, nullptr)))
        Msg("!![%s] Can't get NtQuerySystemInformation", __FUNCTION__);

    DWORD dwTickCount = GetTickCount();
    if (!m_dwCount)
        m_dwCount = dwTickCount;

    for (DWORD i = 0; i < m_dwNumberOfProcessors; i++)
    {
        auto& cpuPerfInfo = perfomanceInfo[i];
        cpuPerfInfo.KernelTime.QuadPart -= cpuPerfInfo.IdleTime.QuadPart;

        fUsage[i] = 100.0f - 0.01f * gsl::narrow_cast<f32>(cpuPerfInfo.IdleTime.QuadPart - m_idleTime[i].QuadPart) / gsl::narrow_cast<f32>(dwTickCount - m_dwCount);
        fUsage[i] = std::clamp(fUsage[i], 0.0f, 100.0f);

        m_idleTime[i] = cpuPerfInfo.IdleTime;
    }

    m_dwCount = dwTickCount;
}
