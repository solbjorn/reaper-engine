#ifndef __XREXTERNAL_ASSERT_H
#define __XREXTERNAL_ASSERT_H

#include "../xrCore/defines.h"

#define LIBASSERT_PREFIX_ASSERTIONS
#define LIBASSERT_SAFE_COMPARISONS
#define LIBASSERT_STATIC_DEFINE
#define LIBASSERT_USE_ENCHANTUM

#ifdef XR_USE_FMT
#define LIBASSERT_NO_STD_FORMAT
#define LIBASSERT_USE_FMT
#endif

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wnan-infinity-disabled");
XR_DIAG_IGNORE("-Wnewline-eof");
XR_DIAG_IGNORE("-Wnrvo");

#include <libassert/assert.hpp>

XR_DIAG_POP();

#endif // !__XREXTERNAL_ASSERT_H
