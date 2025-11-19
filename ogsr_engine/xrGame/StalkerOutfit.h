///////////////////////////////////////////////////////////////
// StalkerOutfit.cpp
// StalkerOutfit - защитный костюм сталкера
///////////////////////////////////////////////////////////////

#pragma once

#include "customoutfit.h"
#include "script_export_space.h"

class CStalkerOutfit : public CCustomOutfit
{
    RTTI_DECLARE_TYPEINFO(CStalkerOutfit, CCustomOutfit);

private:
    typedef CCustomOutfit inherited;

public:
    CStalkerOutfit();
    ~CStalkerOutfit() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CStalkerOutfit);

add_to_type_list(CStalkerOutfit);
#undef script_type_list
#define script_type_list save_type_list(CStalkerOutfit)
