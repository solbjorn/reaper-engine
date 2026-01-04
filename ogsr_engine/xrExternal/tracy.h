#ifndef __XREXTERNAL_TRACY_H
#define __XREXTERNAL_TRACY_H

#define TRACY_ON_DEMAND
#define TRACY_NO_FRAME_IMAGE

#ifndef XR_TRACY_BUILDING
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wsign-conversion");

#include <tracy/Tracy.hpp>

XR_DIAG_POP();

#ifdef TRACY_ENABLE

#ifdef TRACY_ALLOW_SHADOW_WARNING

#define XR_TRACY_ZONE_SCOPED() ZoneScoped

#else // !TRACY_ALLOW_SHADOW_WARNING

#define XR_TRACY_ZONE_SCOPED() ZoneScoped XR_MACRO_END()

#endif // !TRACY_ALLOW_SHADOW_WARNING

#define XR_TRACY_FRAME_MARK() FrameMark

#else // !TRACY_ENABLE

#define XR_TRACY_ZONE_SCOPED() XR_MACRO_END()
#define XR_TRACY_FRAME_MARK() XR_MACRO_END()

#endif // !TRACY_ENABLE

#endif // !XR_TRACY_BUILDING

#endif // !__XREXTERNAL_TRACY_H
