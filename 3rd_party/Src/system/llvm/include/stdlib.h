#ifndef __LLVM_LIBC_STDLIB_H
#define __LLVM_LIBC_STDLIB_H

#undef __restrict

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define __restrict restrict
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    long long quot;
    long long rem;
} lldiv_t;

double atof(const char* str);
int atoi(const char* str);
lldiv_t lldiv(long long x, long long y);
double strtod(const char* __restrict str, char** __restrict str_end);
float strtof(const char* __restrict str, char** __restrict str_end);
long strtol(const char* __restrict str, char** __restrict str_end, int base);
long double strtold(const char* __restrict str, char** __restrict str_end);
long long strtoll(const char* __restrict str, char** __restrict str_end, int base);
unsigned long strtoul(const char* __restrict str, char** __restrict str_end, int base);
unsigned long long strtoull(const char* __restrict str, char** __restrict str_end, int base);

#ifdef __cplusplus
} // extern "C"
#endif

#define _lldiv_t crt__lldiv_t
#define atof crt_atof
#define atoi crt_atoi
#define lldiv crt_lldiv
#define lldiv_t crt_lldiv_t
#define strtod crt_strtod
#define strtof crt_strtof
#define strtol crt_strtol
#define strtold crt_strtold
#define strtoll crt_strtoll
#define strtoul crt_strtoul
#define strtoull crt_strtoull

#include_next <stdlib.h>

#undef strtoull
#undef strtoul
#undef strtoll
#undef strtold
#undef strtol
#undef strtof
#undef strtod
#undef lldiv_t
#undef lldiv
#undef atoi
#undef atof
#undef _lldiv_t

#undef __restrict

#endif // !__LLVM_LIBC_STDLIB_H
