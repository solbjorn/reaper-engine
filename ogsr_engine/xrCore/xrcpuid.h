#pragma once

#include "bitmap.h"
#include <winternl.h>

struct _processor_info final
{
    _processor_info();

    string32 vendor; // vendor name
    string64 brand; // Name of model eg. Intel_Pentium_Pro
    u32 family; // family of the processor, eg. Intel_Pentium_Pro is family 6 processor
    u32 model; // model of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
    u32 stepping; // Processor revision number
    u32 threadCount; // number of available threads, both physical and logical
    u32 coresCount; // number of physical cores

    void clearFeatures()
    {
        m_f1_ECX.zero();
        m_f1_EDX.zero();
        m_f81_ECX.zero();
        m_f81_EDX.zero();
    }

    bool hasMMX() const { return m_f1_EDX.test(23); }
    bool has3DNOWExt() const { return m_f81_EDX.test(30); }
    bool has3DNOW() const { return m_f81_EDX.test(31); }
    bool hasMWAIT() const { return m_f1_ECX.test(3); }
    bool hasSSE4a() const { return m_f81_ECX.test(6); }

    // Always true for AVX2 processors
    constexpr bool hasSSE() const { return true; }
    constexpr bool hasSSE2() const { return true; }
    constexpr bool hasSSE3() const { return true; }
    constexpr bool hasSSSE3() const { return true; }
    constexpr bool hasSSE41() const { return true; }
    constexpr bool hasSSE42() const { return true; }
    constexpr bool hasAVX() const { return true; }
    constexpr bool hasAVX2() const { return true; }

    bool getCPULoad(double& val);
    void MTCPULoad();
    DWORD m_dwNumberOfProcessors;
    std::unique_ptr<float[]> fUsage;

private:
    xr_bitmap<32> m_f1_ECX;
    xr_bitmap<32> m_f1_EDX;
    xr_bitmap<32> m_f81_ECX;
    xr_bitmap<32> m_f81_EDX;

    DWORD m_dwCount = 0;
    FILETIME prevSysIdle{}, prevSysKernel{}, prevSysUser{};
    std::unique_ptr<LARGE_INTEGER[]> m_idleTime;
    std::unique_ptr<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[]> perfomanceInfo;
};
