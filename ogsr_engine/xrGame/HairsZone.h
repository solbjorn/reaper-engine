#pragma once

#include "CustomZone.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "ZoneVisual.h"

#include "script_export_space.h"

class CHairsZone : public CVisualZone
{
    RTTI_DECLARE_TYPEINFO(CHairsZone, CVisualZone);

public:
    typedef CVisualZone inherited;

    CHairsZone() = default;
    ~CHairsZone() override = default;

    void Affect(SZoneObjectInfo* O) override;
    void Load(gsl::czstring section) override;

protected:
    f32 m_min_speed_to_react{};

    [[nodiscard]] bool BlowoutState() override;
    void CheckForAwaking() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CHairsZone);

add_to_type_list(CHairsZone);
#undef script_type_list
#define script_type_list save_type_list(CHairsZone)
