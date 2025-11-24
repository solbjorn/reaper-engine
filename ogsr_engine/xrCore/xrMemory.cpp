#include "stdafx.h"

#include "xrsharedmem.h"

#ifndef __SANITIZE_ADDRESS__
#define USE_MIMALLOC
#endif

#ifdef USE_MIMALLOC
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <mimalloc-override.h>
#include <mimalloc-new-delete.h>

XR_DIAG_POP();

#pragma comment(lib, "mimalloc.dll.lib")
#endif

#include <psapi.h>

void xrMemory::mem_compact()
{
#ifndef USE_MIMALLOC
    /*
    Следующая команда, в целом, не нужна.
    Современные аллокаторы достаточно грамотно и когда нужно возвращают память операционной системе.
    Эта строчка нужна, скорее всего, в определённых ситуациях, вроде использования файлов отображаемых в память,
    которые требуют большие свободные области памяти.
    */
    _heapmin(); //-V530
    HeapCompact(GetProcessHeap(), 0);
#endif

    str_container::clean();
    smem_container::clean();
}

XR_RESTRICT void* xrMemory::mem_alloc_aligned(gsl::index size, gsl::index align) noexcept { return _aligned_malloc(gsl::narrow<size_t>(size), gsl::narrow<size_t>(align)); }

void* xrMemory::mem_realloc_aligned(void* P, gsl::index size, gsl::index align) noexcept { return _aligned_realloc(P, gsl::narrow<size_t>(size), gsl::narrow<size_t>(align)); }

void xrMemory::mem_free_aligned(void* P) noexcept { _aligned_free(P); }

gsl::index xrMemory::mem_usage(gsl::index* pBlocksUsed, gsl::index* pBlocksFree) { return mem_usage_impl(pBlocksUsed, pBlocksFree); }

void GetProcessMemInfo(SProcessMemInfo& minfo)
{
    std::memset(&minfo, 0, sizeof(SProcessMemInfo));

    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    GlobalMemoryStatusEx(&mem);

    minfo.TotalPhysicalMemory = mem.ullTotalPhys;
    minfo.FreePhysicalMemory = mem.ullAvailPhys;

    minfo.TotalPageFile = mem.ullTotalPageFile - mem.ullTotalPhys;

    if (minfo.TotalPageFile > 0l)
    {
        // эта херня погоду показывает на самом деле. надо найти способ как получить свободный размер файла подкачки
        minfo.FreePageFile = mem.ullAvailPageFile > mem.ullAvailPhys ? mem.ullAvailPageFile - mem.ullAvailPhys : mem.ullAvailPageFile;
    }

    minfo.MemoryLoad = mem.dwMemoryLoad;

    PROCESS_MEMORY_COUNTERS pc;
    std::memset(&pc, 0, sizeof(PROCESS_MEMORY_COUNTERS));
    pc.cb = sizeof(pc);
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pc, sizeof(pc)))
    {
        minfo.WorkingSetSize = pc.WorkingSetSize;
        minfo.PeakWorkingSetSize = pc.PeakWorkingSetSize;

        minfo.PagefileUsage = pc.PagefileUsage;
        minfo.PeakPagefileUsage = pc.PeakPagefileUsage;
    }

#ifdef USE_MIMALLOC
    Log("####################[+MIMALLOC+]####################");
    mi_stats_print_out(
        [](const char* msg, void*) {
            std::string str{msg};
            xr_string_utils::rtrim(str);
            Log(str);
        },
        nullptr);
    Log("####################[-MIMALLOC-]####################");
#endif
}

gsl::index mem_usage_impl(gsl::index* pBlocksUsed, gsl::index* pBlocksFree)
{
    static const bool no_memory_usage = !strstr(Core.Params, "-memory_usage");
    if (no_memory_usage)
        return 0;

    _HEAPINFO hinfo;
    int heapstatus;
    hinfo._pentry = nullptr;

    gsl::index total{};
    gsl::index blocks_free{};
    gsl::index blocks_used{};

    while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
    {
        if (hinfo._useflag == _USEDENTRY)
        {
            total += hinfo._size;
            blocks_used += 1;
        }
        else
        {
            blocks_free += 1;
        }
    }

    if (pBlocksFree)
        *pBlocksFree = 1024 * blocks_free;
    if (pBlocksUsed)
        *pBlocksUsed = 1024 * blocks_used;

    switch (heapstatus)
    {
    case _HEAPEMPTY: break;
    case _HEAPEND: break;
    case _HEAPBADPTR: Msg("!![%s] bad pointer to heap", __FUNCTION__); break;
    case _HEAPBADBEGIN: Msg("!![%s] bad start of heap", __FUNCTION__); break;
    case _HEAPBADNODE: Msg("!![%s] bad node in heap", __FUNCTION__); break;
    }
    return total;
}
