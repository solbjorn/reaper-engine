////////////////////////////////////////////////////////////////////////////
//	Module 		: script_hit.h
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script hit class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "script_export_space.h"

class CScriptGameObject;

class CScriptHit
{
public:
    float m_fPower{100.f};
    Fvector m_tDirection{1.f, 0.f, 0.f};
    shared_str m_caBoneName;
    CScriptGameObject* m_tpDraftsman{};
    float m_fImpulse{100.f};
    int m_tHitType{ALife::eHitTypeWound};

public:
    IC CScriptHit();

    IC void set_bone_name(LPCSTR bone_name);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CScriptHit);
#undef script_type_list
#define script_type_list save_type_list(CScriptHit)

#include "script_hit_inline.h"
