#ifndef __XRCORE_DEFINES_H
#define __XRCORE_DEFINES_H

// Definitions shared by the engine and the 3rd party "implementation headers",
// which can't include the precompiled header.

// KRodin: это инклудить только здесь и нигде больше!
#if __has_include("..\build_config_overrides\build_config_defines.h")
#include "..\build_config_overrides\build_config_defines.h"
#else
#include "..\build_config_defines.h"
#endif

// Require function-like macros to end with a ';'
#define XR_MACRO_END() static_assert(true, "")

#define __XR_DIAG_STR(s) #s
#define XR_DIAG_PUSH() _Pragma(__XR_DIAG_STR(clang diagnostic push)) XR_MACRO_END()
#define XR_DIAG_IGNORE(s) _Pragma(__XR_DIAG_STR(clang diagnostic ignored s)) XR_MACRO_END()
#define XR_DIAG_POP() _Pragma(__XR_DIAG_STR(clang diagnostic pop)) XR_MACRO_END()

#endif // __XRCORE_DEFINES_H
