#ifndef __LLVM_LIBC_STRING_H
#define __LLVM_LIBC_STRING_H

#undef __restrict

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define __restrict restrict
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned __int64 size_t;

#undef memccpy
void* memccpy(void* __restrict dest, const void* __restrict src, int c, size_t count);

#undef strcasecmp
int strcasecmp(const char* left, const char* right);

size_t strcspn(const char* src, const char* segment);

#undef strdup
char* strdup(const char* src);

#undef strncasecmp
int strncasecmp(const char* left, const char* right, size_t n);

char* strncat(char* __restrict dest, const char* __restrict src, size_t count);
int strncmp(const char* left, const char* right, size_t n);
char* strncpy(char* __restrict dest, const char* __restrict src, size_t n);
size_t strnlen(const char* src, size_t n);
size_t strspn(const char* src, const char* segment);
char* strtok(char* __restrict src, const char* __restrict delimiter_string);

static __attribute__((always_inline)) __attribute__((__unused__)) int _stricmp(const char* left, const char* right) { return strcasecmp(left, right); }

static __attribute__((always_inline)) __attribute__((__unused__)) int _strnicmp(const char* left, const char* right, size_t n)
{
    return strncasecmp(left, right, n);
}

static __attribute__((always_inline)) __attribute__((__unused__)) size_t strnlen_s(const char* src, size_t n)
{
    if (__builtin_expect(src == 0, 0))
        return 0;

    return strnlen(src, n);
}

#ifdef __cplusplus
} // extern "C"
#endif

#define _memccpy dontuse_crt__memccpy
#define _strdup dontuse_crt__strdup
#define _stricmp dontuse_crt__stricmp
#define _strnicmp dontuse_crt__strnicmp
#define memccpy dontuse_crt_memccpy
#define strcspn dontuse_crt_strcspn
#define strdup dontuse_crt_strdup
#define strncat dontuse_crt_strncat
#define strncmp dontuse_crt_strncmp
#define strncpy dontuse_crt_strncpy
#define strnlen dontuse_crt_strnlen
#define strnlen_s dontuse_crt_strnlen_s
#define strspn dontuse_crt_strspn
#define strtok dontuse_crt_strtok

#include_next <string.h>

#undef strtok
#undef strspn
#undef strnlen_s
#undef strnlen
#undef strncpy
#undef strncmp
#undef strncat
#undef strdup
#undef strcspn
#undef memccpy
#undef _strnicmp
#undef _stricmp
#undef _strdup
#undef _memccpy

#define _memccpy memccpy
#define _strdup strdup

#undef __restrict

#endif // !__LLVM_LIBC_STRING_H
