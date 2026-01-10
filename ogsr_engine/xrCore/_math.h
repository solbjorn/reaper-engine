#pragma once

#include <thread>

struct _processor_info;

namespace CPU
{
extern s64 qpc_freq;
extern u32 qpc_counter;
extern _processor_info ID;

[[nodiscard]] s64 QPC();
} // namespace CPU

void _initialize_cpu();
