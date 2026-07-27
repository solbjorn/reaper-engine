#pragma once

struct _processor_info;

namespace CPU
{
extern _processor_info ID;

extern s64 qpc_freq;
extern u32 qpc_counter;

[[nodiscard]] s64 QPC();
} // namespace CPU
