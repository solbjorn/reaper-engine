#pragma once

struct _processor_info;

namespace CPU
{
extern u64 qpc_freq;
extern u32 qpc_counter;

extern _processor_info ID;
extern u64 QPC();
} // namespace CPU

void _initialize_cpu();
void set_current_thread_name(const char* threadName);
void set_thread_name(const char* threadName, std::thread& thread);
