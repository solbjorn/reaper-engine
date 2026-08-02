#ifndef __LLVM_LIBC_CORECRT_WCTYPE_H
#define __LLVM_LIBC_CORECRT_WCTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

int iswspace(wchar_t c);

#ifdef __cplusplus
} // extern "C"
#endif

#define iswspace dontuse_crt_iswspace

#include_next <corecrt_wctype.h>

#undef iswspace

#endif // !__LLVM_LIBC_CORECRT_WCTYPE_H
