#ifndef __LLVM_LIBC_CTYPE_H
#define __LLVM_LIBC_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

int isalnum(int c);
int isalpha(int c);
int isblank(int c);
int iscntrl(int c);
int isprint(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int tolower(int c);
int toupper(int c);

#ifdef __cplusplus
} // extern "C"
#endif

#define isalnum dontuse_crt_isalnum
#define isalpha dontuse_crt_isalpha
#define isblank dontuse_crt_isblank
#define iscntrl dontuse_crt_iscntrl
#define isprint dontuse_crt_isprint
#define isspace dontuse_crt_isspace
#define isupper dontuse_crt_isupper
#define isxdigit dontuse_crt_isxdigit
#define tolower dontuse_crt_tolower
#define toupper dontuse_crt_toupper

#include_next <ctype.h>

#undef toupper
#undef tolower
#undef isxdigit
#undef isupper
#undef isspace
#undef isprint
#undef iscntrl
#undef isblank
#undef isalpha
#undef isalnum

#endif // !__LLVM_LIBC_CTYPE_H
