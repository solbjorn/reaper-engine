#pragma once

#include "grenade.h"
#include "script_export_space.h"

class CRGD5 : public CGrenade
{
    RTTI_DECLARE_TYPEINFO(CRGD5, CGrenade);

public:
    typedef CGrenade inherited;

    CRGD5();
    virtual ~CRGD5();

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CRGD5);

add_to_type_list(CRGD5);
#undef script_type_list
#define script_type_list save_type_list(CRGD5)
