#include "stdafx.h"

#include "PHInterpolation.h"

#include "PHDynamicData.h"
#include "Physics.h"
#include "MathUtils.h"

extern CPHWorld* ph_world;

void CPHInterpolation::SetBody(dBodyID body)
{
    if (!body)
        return;
    m_body = body;
    qPositions.fill_in(*((const Fvector*)dBodyGetPosition(m_body)));
    const dReal* dQ = dBodyGetQuaternion(m_body);
    Fquaternion fQ;
    fQ.set(-dQ[0], dQ[1], dQ[2], dQ[3]);
    qRotations.fill_in(fQ);
}

void CPHInterpolation::UpdatePositions()
{
    auto& pos = qPositions.emplace_back();
    pos = *((const Fvector*)dBodyGetPosition(XR_ASSERT_VAL(m_body != nullptr)));
}

void CPHInterpolation::UpdateRotations()
{
    const dReal* dQ = dBodyGetQuaternion(XR_ASSERT_VAL(m_body != nullptr));
    auto& fQ = qRotations.emplace_back();
    fQ.set(-dQ[0], dQ[1], dQ[2], dQ[3]);
}

void CPHInterpolation::InterpolatePosition(Fvector& pos) { pos.lerp(qPositions[0], qPositions[1], ph_world->m_frame_time / fixed_step); }

void CPHInterpolation::InterpolateRotation(Fmatrix& rot)
{
    const f32 t = ph_world->m_frame_time / fixed_step;
    XR_ASSERT(t >= 0.0f && t <= 1.0f, "", t);

    Fquaternion q;
    q.slerp(qRotations[0], qRotations[1], t);
    rot.rotation(q);
}

void CPHInterpolation::ResetPositions()
{
    XR_DEBUG_ASSERT(dBodyStateValide(m_body));
    qPositions.fill_in(*((const Fvector*)dBodyGetPosition(m_body)));
}

void CPHInterpolation::ResetRotations()
{
    XR_DEBUG_ASSERT(dBodyStateValide(m_body));

    const dReal* dQ = dBodyGetQuaternion(m_body);
    Fquaternion fQ;
    fQ.set(-dQ[0], dQ[1], dQ[2], dQ[3]);
    qRotations.fill_in(fQ);
}

void CPHInterpolation::GetRotation(Fquaternion& q, u16 num)
{
    if (!m_body)
        return;
    q.set(qRotations[num]);
}

void CPHInterpolation::GetPosition(Fvector& p, u16 num)
{
    if (!m_body)
        return;
    p.set(qPositions[num]);
}

void CPHInterpolation::SetPosition(const Fvector& p, u16 num)
{
    if (!m_body)
        return;
    qPositions[num].set(p);
}

void CPHInterpolation::SetRotation(const Fquaternion& q, u16 num)
{
    if (!m_body)
        return;
    qRotations[num] = q;
}
