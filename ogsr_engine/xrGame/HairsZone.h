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

    virtual void Affect(SZoneObjectInfo* O);
    virtual void Load(LPCSTR section);

protected:
    float m_min_speed_to_react;

    virtual bool BlowoutState();
    virtual void CheckForAwaking();

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CHairsZone);

add_to_type_list(CHairsZone);
#undef script_type_list
#define script_type_list save_type_list(CHairsZone)
