#include "stdafx.h"

#include "xrcpuid.h"

#include <VersionHelpers.h>
#include <powerbase.h>

#pragma comment(lib, "PowrProf.lib")

typedef struct _PROCESSOR_POWER_INFORMATION
{
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

// Initialized on startup
const Fmatrix Fidentity{Fmatrix{}.identity()};

namespace CPU
{
s64 qpc_freq;
u32 qpc_counter{};
_processor_info ID;

s64 QPC()
{
    LARGE_INTEGER _dest;
    QueryPerformanceCounter(&_dest);
    qpc_counter++;

    return _dest.QuadPart;
}
} // namespace CPU

void _initialize_cpu()
{
    Msg("* Detected CPU: %s [%s], F%u/M%u/S%u", CPU::ID.brand, CPU::ID.vendor, CPU::ID.family, CPU::ID.model, CPU::ID.stepping);

    string256 features;
    xr_strcpy(features, "* CPU features: RDTSC");

    if (CPU::ID.hasMMX())
        xr_strcat(features, ", MMX");
    if (CPU::ID.has3DNOWExt())
        xr_strcat(features, ", 3DNowExt!");
    if (CPU::ID.has3DNOW())
        xr_strcat(features, ", 3DNow!");
    if (CPU::ID.hasSSE())
        xr_strcat(features, ", SSE");
    if (CPU::ID.hasSSE2())
        xr_strcat(features, ", SSE2");
    if (CPU::ID.hasSSE3())
        xr_strcat(features, ", SSE3");
    if (CPU::ID.hasMWAIT())
        xr_strcat(features, ", MONITOR/MWAIT");
    if (CPU::ID.hasSSSE3())
        xr_strcat(features, ", SSSE3");
    if (CPU::ID.hasSSE41())
        xr_strcat(features, ", SSE4.1");
    if (CPU::ID.hasSSE42())
        xr_strcat(features, ", SSE4.2");
    if (CPU::ID.hasSSE4a())
        xr_strcat(features, ", SSE4a");
    if (CPU::ID.hasAVX())
        xr_strcat(features, ", AVX");
    if (CPU::ID.hasAVX2())
        xr_strcat(features, ", AVX2");

    Log(features);

    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    CPU::qpc_freq = Freq.QuadPart;

    CPU::ID.print_topology();
}
