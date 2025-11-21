#pragma once

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

    constexpr void clearFeatures()
    {
        m_f1_ECX.reset();
        m_f1_EDX.reset();
        m_f81_ECX.reset();
        m_f81_EDX.reset();
    }

    [[nodiscard]] constexpr bool hasMMX() const { return m_f1_EDX.test(23); }
    [[nodiscard]] constexpr bool has3DNOWExt() const { return m_f81_EDX.test(30); }
    [[nodiscard]] constexpr bool has3DNOW() const { return m_f81_EDX.test(31); }
    [[nodiscard]] constexpr bool hasMWAIT() const { return m_f1_ECX.test(3); }
    [[nodiscard]] constexpr bool hasSSE4a() const { return m_f81_ECX.test(6); }

    // Always true for AVX2 processors
    [[nodiscard]] static constexpr bool hasSSE() { return true; }
    [[nodiscard]] static constexpr bool hasSSE2() { return true; }
    [[nodiscard]] static constexpr bool hasSSE3() { return true; }
    [[nodiscard]] static constexpr bool hasSSSE3() { return true; }
    [[nodiscard]] static constexpr bool hasSSE41() { return true; }
    [[nodiscard]] static constexpr bool hasSSE42() { return true; }
    [[nodiscard]] static constexpr bool hasAVX() { return true; }
    [[nodiscard]] static constexpr bool hasAVX2() { return true; }

    [[nodiscard]] bool getCPULoad(f64& val);
    void MTCPULoad();
    DWORD m_dwNumberOfProcessors;
    std::unique_ptr<float[]> fUsage;

private:
    xr::bitset<32, u32> m_f1_ECX;
    xr::bitset<32, u32> m_f1_EDX;
    xr::bitset<32, u32> m_f81_ECX;
    xr::bitset<32, u32> m_f81_EDX;

    DWORD m_dwCount = 0;
    FILETIME prevSysIdle{}, prevSysKernel{}, prevSysUser{};
    std::unique_ptr<LARGE_INTEGER[]> m_idleTime;
    std::unique_ptr<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[]> perfomanceInfo;
};
