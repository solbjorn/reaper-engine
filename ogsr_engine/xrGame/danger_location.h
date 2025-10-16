////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_location.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "memory_space.h"

class CObject;

class XR_NOVTABLE CDangerLocation : public intrusive_base
{
    RTTI_DECLARE_TYPEINFO(CDangerLocation, intrusive_base);

public:
    typedef MemorySpace::squad_mask_type squad_mask_type;
    typedef _flags<squad_mask_type> flags;

    u32 m_level_time;
    u32 m_interval;
    float m_radius;
    flags m_mask;

    inline bool operator==(const Fvector& position) const;
    inline virtual bool operator==(const CObject*) const;

    virtual bool useful() const;
    virtual const Fvector& position() const = 0;
    inline const flags& mask() const;
};

#include "danger_location_inline.h"
