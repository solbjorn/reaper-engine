#include "pch.h"

#include "tracy.h"

#ifdef TRACY_ENABLE
#define fileno _fileno
#define fstat _fstat
#define stat _stat
#endif

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wcast-qual");
XR_DIAG_IGNORE("-Wcomma");
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wformat");
XR_DIAG_IGNORE("-Wimplicit-int-conversion");
XR_DIAG_IGNORE("-Wimplicit-int-float-conversion");
XR_DIAG_IGNORE("-Wmissing-field-initializers");
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wmissing-variable-declarations");
XR_DIAG_IGNORE("-Wnewline-eof");
XR_DIAG_IGNORE("-Wnrvo");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wshorten-64-to-32");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wunused-function");
XR_DIAG_IGNORE("-Wunused-parameter");
XR_DIAG_IGNORE("-Wunused-value");
XR_DIAG_IGNORE("-Wunused-variable");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <TracyClient.cpp>

XR_DIAG_POP();
