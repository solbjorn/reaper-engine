#include "stdafx.h"
#include "xrcpuid.h"
#include <intrin.h>

decltype(auto) countSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
    DWORD bitSetCount = 0;
    auto bitTest = static_cast<ULONG_PTR>(1) << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest) ? 1 : 0);
        bitTest /= 2;
    }

    return bitSetCount;
}

static ICF void xr_cpuid(u32* regs, u32 leaf)
{
#ifdef __clang__
    __cpuid(leaf, regs[0], regs[1], regs[2], regs[3]);
#else
    __cpuid(regs, leaf);
#endif
}

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
    model = (u8)((cpinfo[0] >> 4) & 0xf) | ((u8)((cpinfo[0] >> 16) & 0xf) << 4);
    family = (u8)((cpinfo[0] >> 8) & 0xf) | ((u8)((cpinfo[0] >> 20) & 0xff) << 4);
    m_f1_ECX.init(cpinfo[2]);
    m_f1_EDX.init(cpinfo[3]);

    // and check 3DNow! support
    xr_cpuid(cpinfo, 0x80000001);
    m_f81_ECX.init(cpinfo[2]);
    m_f81_EDX.init(cpinfo[3]);

    // get version of OS
    DWORD dwMajorVersion = 0;
    DWORD dwVersion = 0;
    dwVersion = GetVersion();

    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    if (dwMajorVersion <= 5) // XP don't support SSE3+ instruction sets
    {
        m_f1_ECX.clear(0);
        m_f1_ECX.clear(9);
        m_f1_ECX.clear(19);
        m_f1_ECX.clear(20);
        m_f1_ECX.clear(28);
        m_f81_ECX.clear(6);
    }

    // Calculate available processors
    DWORD returnedLength = 0;
    DWORD byteOffset = 0;
    GetLogicalProcessorInformation(nullptr, &returnedLength);

    auto buffer = std::make_unique<u8[]>(returnedLength);
    auto ptr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer.get());
    GetLogicalProcessorInformation(ptr, &returnedLength);

    auto processorCoreCount = 0u;
    auto logicalProcessorCount = 0u;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnedLength)
    {
        switch (ptr->Relationship)
        {
        case RelationProcessorCore:
            processorCoreCount++;

            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += countSetBits(ptr->ProcessorMask);
            break;

        default: break;
        }

        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

    // All logical processors
    coresCount = processorCoreCount;
    threadCount = logicalProcessorCount;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    m_dwNumberOfProcessors = sysInfo.dwNumberOfProcessors;
    fUsage = std::make_unique<float[]>(m_dwNumberOfProcessors);
    m_idleTime = std::make_unique<LARGE_INTEGER[]>(m_dwNumberOfProcessors);
    perfomanceInfo = std::make_unique<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[]>(m_dwNumberOfProcessors);
}

unsigned long long SubtractTimes(const FILETIME one, const FILETIME two)
{
    LARGE_INTEGER a, b;
    a.LowPart = one.dwLowDateTime;
    a.HighPart = one.dwHighDateTime;

    b.LowPart = two.dwLowDateTime;
    b.HighPart = two.dwHighDateTime;

    return a.QuadPart - b.QuadPart;
}

bool _processor_info::getCPULoad(double& val)
{
    FILETIME sysIdle, sysKernel, sysUser;
    // sysKernel include IdleTime
    if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0) // GetSystemTimes func FAILED return value is zero;
        return false;

    if (prevSysIdle.dwLowDateTime != 0 && prevSysIdle.dwHighDateTime != 0)
    {
        DWORDLONG sysIdleDiff, sysKernelDiff, sysUserDiff;
        sysIdleDiff = SubtractTimes(sysIdle, prevSysIdle);
        sysKernelDiff = SubtractTimes(sysKernel, prevSysKernel);
        sysUserDiff = SubtractTimes(sysUser, prevSysUser);

        DWORDLONG sysTotal = sysKernelDiff + sysUserDiff;
        DWORDLONG kernelTotal = sysKernelDiff - sysIdleDiff; // kernelTime - IdleTime = kernelTime, because sysKernel include IdleTime

        if (sysTotal > 0) // sometimes kernelTime > idleTime
            val = (double)(((kernelTotal + sysUserDiff) * 100.0) / sysTotal);
    }

    prevSysIdle = sysIdle;
    prevSysKernel = sysKernel;
    prevSysUser = sysUser;

    return true;
}

void _processor_info::MTCPULoad()
{
    using NTQUERYSYSTEMINFORMATION = NTSTATUS(NTAPI*)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
    static auto m_pNtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");

    if (!NT_SUCCESS(m_pNtQuerySystemInformation(SystemProcessorPerformanceInformation, perfomanceInfo.get(),
                                                sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * (ULONG)m_dwNumberOfProcessors, nullptr)))
        Msg("!![%s] Can't get NtQuerySystemInformation", __FUNCTION__);

    DWORD dwTickCount = GetTickCount();
    if (!m_dwCount)
        m_dwCount = dwTickCount;

    for (DWORD i = 0; i < m_dwNumberOfProcessors; i++)
    {
        auto* cpuPerfInfo = &perfomanceInfo[i];
        cpuPerfInfo->KernelTime.QuadPart -= cpuPerfInfo->IdleTime.QuadPart;

        fUsage[i] = 100.0f - 0.01f * (cpuPerfInfo->IdleTime.QuadPart - m_idleTime[i].QuadPart) / ((dwTickCount - m_dwCount));
        if (fUsage[i] < 0.0f)
        {
            fUsage[i] = 0.0f;
        }
        if (fUsage[i] > 100.0f)
        {
            fUsage[i] = 100.0f;
        }

        m_idleTime[i] = cpuPerfInfo->IdleTime;
    }

    m_dwCount = dwTickCount;
}
