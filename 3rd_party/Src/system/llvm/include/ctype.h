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

#define isalnum crt_isalnum
#define isalpha crt_isalpha
#define isblank crt_isblank
#define iscntrl crt_iscntrl
#define isprint crt_isprint
#define isspace crt_isspace
#define isupper crt_isupper
#define isxdigit crt_isxdigit
#define tolower crt_tolower
#define toupper crt_toupper

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
