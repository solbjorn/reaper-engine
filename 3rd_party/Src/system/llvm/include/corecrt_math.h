#ifndef __LLVM_LIBC_CORECRT_MATH_H
#define __LLVM_LIBC_CORECRT_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

float acosf(float x);
float asinf(float x);
float atan2f(float x, float y);
float atanf(float x);
double atof(const char* str);

#undef copysign
double copysign(double x, double y);

float cosf(float x);
float expf(float x);
float fmodf(float x, float y);
double frexp(double x, int* exp);
float frexpf(float x, int* exp);
double hypot(double x, double y);

#undef hypotf
float hypotf(float x, float y);

double ldexp(double x, int exp);
float ldexpf(float x, int exp);
long double ldexpl(long double x, int exp);
long long llround(double x);
float log10f(float x);
double log2(double x);
float log2f(float x);
double logb(double x);
float logf(float x);
long lrintf(float x);
long lround(double x);
long lroundf(float x);
float nextafterf(float x, float y);
float powf(float x, float y);
double scalbn(double x, int n);
float sinf(float x);
float sqrtf(float x);
float tanf(float x);

#ifdef __cplusplus
} // extern "C"
#endif

#define _copysign crt__copysign
#define _hypotf crt__hypotf
#define acosf crt_acosf
#define asinf crt_asinf
#define atan2f crt_atan2f
#define atanf crt_atanf
#define atof crt_atof
#define copysign crt_copysign
#define cosf crt_cosf
#define expf crt_expf
#define fmodf crt_fmodf
#define frexp crt_frexp
#define frexpf crt_frexpf
#define hypot crt_hypot
#define hypotf crt_hypotf
#define ldexp crt_ldexp
#define ldexpf crt_ldexpf
#define ldexpl crt_ldexpl
#define llround crt_llround
#define log10f crt_log10f
#define log2 crt_log2
#define log2f crt_log2f
#define logb crt_logb
#define logf crt_logf
#define lrintf crt_lrintf
#define lround crt_lround
#define lroundf crt_lroundf
#define nextafterf crt_nextafterf
#define powf crt_powf
#define scalbn crt_scalbn
#define sinf crt_sinf
#define sqrtf crt_sqrtf
#define tanf crt_tanf

#include_next <corecrt_math.h>

#undef tanf
#undef sqrtf
#undef sinf
#undef scalbn
#undef powf
#undef nextafterf
#undef lroundf
#undef lround
#undef lrintf
#undef logf
#undef logb
#undef log2f
#undef log2
#undef log10f
#undef llround
#undef ldexpl
#undef ldexpf
#undef ldexp
#undef hypotf
#undef hypot
#undef frexpf
#undef frexp
#undef fmodf
#undef expf
#undef cosf
#undef copysign
#undef atof
#undef atanf
#undef atan2f
#undef asinf
#undef acosf
#undef _hypotf
#undef _copysign

#define _copysign copysign
#define _hypotf hypotf

#endif // !__LLVM_LIBC_CORECRT_MATH_H
