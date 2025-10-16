////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_object_location.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger object location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "danger_location.h"

class CGameObject;

class CDangerObjectLocation : public CDangerLocation
{
    RTTI_DECLARE_TYPEINFO(CDangerObjectLocation, CDangerLocation);

private:
    const CGameObject* m_object;

public:
    IC CDangerObjectLocation(const CGameObject* object, u32 level_time, u32 interval, float radius, const squad_mask_type& mask = squad_mask_type(-1));

    bool operator==(const CObject* object) const override;

    const Fvector& position() const override;
    bool useful() const override;
};

#include "danger_object_location_inline.h"
