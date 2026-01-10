#include "pch.h"

#include "tmc.h"

#pragma push_macro("_USRDLL")
#undef _USRDLL

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wdeprecated-declarations");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wunused-parameter");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <hwloc.h>

XR_DIAG_POP();

#pragma pop_macro("_USRDLL")

#include <tmc/all_headers.hpp>
