#include "stdafx.h"

#include "bone.h"

#include "GameMtlLib.h"

//////////////////////////////////////////////////////////////////////////
// BoneInstance methods

void CBoneInstance::set_param(u32 idx, float data)
{
    VERIFY(idx < MAX_BONE_PARAMS);
    param[idx] = data;
}
float CBoneInstance::get_param(u32 idx)
{
    VERIFY(idx < MAX_BONE_PARAMS);
    return param[idx];
}

#ifdef DEBUG
void CBoneData::DebugQuery(BoneDebug& L)
{
    for (u32 i = 0; i < children.size(); i++)
    {
        L.push_back(SelfID);
        L.push_back(children[i]->SelfID);
        children[i]->DebugQuery(L);
    }
}
#endif

void CBoneData::CalculateM2B(const Fmatrix& parent)
{
    // Build matrix
    m2b_transform.mul_43(parent, bind_transform);

    // Calculate children
    for (xr_vector<CBoneData*>::iterator C = children.begin(); C != children.end(); C++)
        (*C)->CalculateM2B(m2b_transform);

    m2b_transform.invert();
}
