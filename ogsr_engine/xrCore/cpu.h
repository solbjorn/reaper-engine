#pragma once

#include <cpu_features/cpuinfo_x86.h>

#include <winternl.h>

struct _processor_info final
{
private:
    cpu_features::X86Features features;

    template <bool compile_time>
    [[nodiscard]] static constexpr bool has(bool runtime)
    {
        if constexpr (compile_time)
            return true;

        return runtime;
    }

public:
    [[nodiscard]] constexpr bool hasSSE() const { return has<CPU_FEATURES_COMPILED_X86_SSE>(features.sse); }

    std::unique_ptr<float[]> fUsage;
    DWORD m_dwNumberOfProcessors;

    [[nodiscard]] bool getCPULoad(f64& val);
    void MTCPULoad();

private:
    DWORD m_dwCount = 0;
    FILETIME prevSysIdle{}, prevSysKernel{}, prevSysUser{};
    std::unique_ptr<LARGE_INTEGER[]> m_idleTime;
    std::unique_ptr<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[]> perfomanceInfo;

    const tmc::ex_any* const cpu{tmc::cpu_executor().type_erased()};
    const tmc::ex_any* const st{xr::tmc_cpu_st_executor().type_erased()};

    void print_features(std::string_view pfx, std::span<std::string_view> list);
    void print_features();
    void print_cache();
    void print_topology();

public:
    xr_vector<tmc::topology::thread_info> threads;
    tmc::topology::cpu_topology topo;

    [[nodiscard]] constexpr bool on_cpu() const { return tmc::current_executor() == cpu; }
    [[nodiscard]] constexpr bool on_st() const { return tmc::current_executor() == st; }

    void init();
    void print();
};
