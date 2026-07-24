#include "StdAfx.h"

#include "ik_limb_state.h"

#include "ik/IKLimb.h"

void ik_limb_state::set_limb(CIKLimb* l)
{
    limb = XR_ASSERT_VAL(l != nullptr);
    state.ref_bone = l->ref_bone();
}

ik_goal_matrix& ik_limb_state::to_ref_bone(ik_goal_matrix& m) const
{
    Fmatrix mt = m.get();
    to_ref_bone(mt);
    m.set(mt, m.collide_state());

    return m;
}

Fmatrix& ik_limb_state::to_ref_bone(Fmatrix& m) const
{
    if (state.ref_bone == XR_ASSERT_VAL(limb != nullptr)->ref_bone())
        return m;

    Fmatrix tobone = state.b2tob3;
    if (state.ref_bone == 2 && limb->ref_bone() == 3)
    {
    }
    else if (state.ref_bone == 3 && limb->ref_bone() == 2)
        tobone.invert();
    else
        XR_PANIC();

    m = Fmatrix().mul_43(m, tobone);

    return m;
}

Fmatrix& ik_limb_state::anim_pos(Fmatrix& m) const
{
    m.set(anim_pos());
    return to_ref_bone(m);
}

ik_goal_matrix& ik_limb_state::goal(ik_goal_matrix& m) const
{
    m = goal();
    return to_ref_bone(m);
}

ik_goal_matrix& ik_limb_state::blend_to(ik_goal_matrix& m) const
{
    m = blend_to();
    return to_ref_bone(m);
}

Fvector& ik_limb_state::pick(Fvector& v) const
{
    if (state.ref_bone == limb->ref_bone())
    {
        v.set(state.pick);
        return v;
    }

    Fmatrix m;
    limb->transform(m, state.ref_bone, limb->ref_bone()).transform_tiny(v, state.pick);

    return v;
}
