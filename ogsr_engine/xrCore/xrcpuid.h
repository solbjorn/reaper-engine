#pragma once

#include <bitset>
#include <winternl.h>

struct XRCORE_API _processor_info final
{
    _processor_info();

    string32 vendor; // vendor name
    string64 brand; // Name of model eg. Intel_Pentium_Pro
    int family; // family of the processor, eg. Intel_Pentium_Pro is family 6 processor
    int model; // model of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
    int stepping; // Processor revision number
    unsigned int threadCount; // number of available threads, both physical and logical
    unsigned int coresCount; // number of physical cores

    void clearFeatures() { m_f1_ECX = m_f1_EDX = m_f81_ECX = m_f81_EDX = 0; }
    bool hasMMX() const { return m_f1_EDX[23]; }
    bool has3DNOWExt() const { return m_f81_EDX[30]; }
    bool has3DNOW() const { return m_f81_EDX[31]; }
    bool hasMWAIT() const { return m_f1_ECX[3]; }
    bool hasSSE4a() const { return m_f81_ECX[6]; }

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
    std::bitset<32> m_f1_ECX;
    std::bitset<32> m_f1_EDX;
    std::bitset<32> m_f81_ECX;
    std::bitset<32> m_f81_EDX;

    DWORD m_dwCount = 0;
    FILETIME prevSysIdle{}, prevSysKernel{}, prevSysUser{};
    std::unique_ptr<LARGE_INTEGER[]> m_idleTime;
    std::unique_ptr<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[]> perfomanceInfo;
};
