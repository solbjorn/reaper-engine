#pragma once

#include "grenade.h"
#include "script_export_space.h"

class CF1 : public CGrenade
{
    RTTI_DECLARE_TYPEINFO(CF1, CGrenade);

public:
    typedef CGrenade inherited;

    CF1();
    virtual ~CF1();

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CF1);

add_to_type_list(CF1);
#undef script_type_list
#define script_type_list save_type_list(CF1)
