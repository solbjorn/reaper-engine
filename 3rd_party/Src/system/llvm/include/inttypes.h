#ifndef __LLVM_LIBC_INTTYPES_H
#define __LLVM_LIBC_INTTYPES_H

#undef __restrict

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define __restrict restrict
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

intmax_t strtoimax(const char* __restrict str, char** __restrict str_end, int base);
uintmax_t strtoumax(const char* __restrict str, char** __restrict str_end, int base);

#ifdef __cplusplus
} // extern "C"
#endif

#define strtoimax crt_strtoimax
#define strtoumax crt_strtoumax

#include_next <inttypes.h>

#undef strtoumax
#undef strtoimax

#undef __restrict

#endif // !__LLVM_LIBC_INTTYPES_H
