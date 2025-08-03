#ifndef __XRSCRIPT_SCRIPT_CAMERA_H
#define __XRSCRIPT_SCRIPT_CAMERA_H

#include "../xr_3da/CameraBase.h"
#include "script_export_space.h"

typedef class_exporter<CCameraBase> CCameraScript;

add_to_type_list(CCameraScript);
#undef script_type_list
#define script_type_list save_type_list(CCameraScript)

#endif /* __XRSCRIPT_SCRIPT_CAMERA_H */
