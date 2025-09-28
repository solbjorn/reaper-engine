////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_effector.h"
#include "actor.h"
#include "ActorEffector.h"

BOOL CScriptEffector::Process(SPPInfo& pp)
{
    auto op = ops.find(effector_ops::PROCESS);
    if (op == ops.end())
        return !!inherited::Process(pp);

    bool ret = op->second(this, pp);
    inherited::Process(pp);

    return ret;
}

void CScriptEffector::Add() { Actor()->Cameras().AddPPEffector(this); }

void CScriptEffector::Remove() { Actor()->Cameras().RemovePPEffector(m_tEffectorType); }
