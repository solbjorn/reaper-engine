#pragma once

extern void rtc_initialize();
extern size_t rtc_compress(void* dst, size_t dst_len, const void* src, size_t src_len);
extern size_t rtc_decompress(void* dst, size_t dst_len, const void* src, size_t src_len);
extern u32 rtc_csize(u32 in);
