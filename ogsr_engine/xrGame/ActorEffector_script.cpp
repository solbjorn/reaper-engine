#include "stdafx.h"

#include "ActorEffector.h"

#include "ai_space.h"
#include "script_engine.h"
#include "../xr_3da/ObjectAnimator.h"

void CAnimatorCamEffectorScriptCB::ProcessIfInvalid(SCamEffectorInfo& info)
{
    if (m_bAbsolutePositioning)
    {
        const Fmatrix& m = m_objectAnimator->XFORM();
        info.d = m.k;
        info.n = m.j;
        info.p = m.c;
        if (m_fov > 0.0f)
            info.fFov = m_fov;
    }
}

BOOL CAnimatorCamEffectorScriptCB::Valid()
{
    BOOL res = inherited::Valid();
    if (!res && !cb_name.empty())
    {
        sol::function fl;
        R_ASSERT(ai().script_engine().function(cb_name.c_str(), fl));

        fl();
        cb_name._set("");
    }

    return res;
}
