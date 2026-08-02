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

#define _lldiv_t dontuse_crt__lldiv_t
#define atof dontuse_crt_atof
#define atoi dontuse_crt_atoi
#define lldiv dontuse_crt_lldiv
#define lldiv_t dontuse_crt_lldiv_t
#define strtod dontuse_crt_strtod
#define strtof dontuse_crt_strtof
#define strtol dontuse_crt_strtol
#define strtold dontuse_crt_strtold
#define strtoll dontuse_crt_strtoll
#define strtoul dontuse_crt_strtoul
#define strtoull dontuse_crt_strtoull

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
