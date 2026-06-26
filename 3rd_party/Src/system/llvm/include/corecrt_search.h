#ifndef __LLVM_LIBC_CORECRT_SEARCH_H
#define __LLVM_LIBC_CORECRT_SEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned __int64 size_t;

void* bsearch(const void* key, const void* array, size_t array_size, size_t elem_size, int (*compare)(const void*, const void*));
void qsort(void* array, size_t array_size, size_t elem_size, int (*compare)(const void*, const void*));

#ifdef __cplusplus
} // extern "C"
#endif

#define bsearch crt_bsearch
#define qsort crt_qsort

#include_next <corecrt_search.h>

#undef qsort
#undef bsearch

#endif // !__LLVM_LIBC_CORECRT_SEARCH_H
