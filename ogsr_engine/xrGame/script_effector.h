////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\xr_3da\effectorpp.h"
#include "script_export_space.h"
#include "..\xr_3da\cameramanager.h"

class CScriptEffector : public CEffectorPP
{
    RTTI_DECLARE_TYPEINFO(CScriptEffector, CEffectorPP);

public:
    typedef CEffectorPP inherited;
    EEffectorPPType m_tEffectorType;

private:
    enum class effector_ops : s32
    {
        PROCESS
    };

    sol::object priv;
    xr_map<effector_ops, sol::function> ops;

public:
    inline explicit CScriptEffector(int iType, float time);
    ~CScriptEffector() override = default;

    virtual BOOL Process(SPPInfo& pp);
    virtual void Add();
    virtual void Remove();

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CScriptEffector);
#undef script_type_list
#define script_type_list save_type_list(CScriptEffector)

#include "script_effector_inline.h"
