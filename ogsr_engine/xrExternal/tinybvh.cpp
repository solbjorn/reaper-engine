#include "pch.h"

#include "tinybvh.h"

#include "assert.h"

#define fprintf(stream, format, line, desc) LIBASSERT_PANIC(desc)

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wcast-qual");
XR_DIAG_IGNORE("-Wcomma");
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");
XR_DIAG_IGNORE("-Wdeprecated-copy-with-user-provided-dtor");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wformat-signedness");
XR_DIAG_IGNORE("-Wimplicit-int-float-conversion");
XR_DIAG_IGNORE("-Wmacro-redefined");
XR_DIAG_IGNORE("-Wmissing-braces");
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wnewline-eof");
XR_DIAG_IGNORE("-Wnrvo");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wunused-variable");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <tiny_bvh.h>

XR_DIAG_POP();

#undef fprintf
