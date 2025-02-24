#ifndef SkeletonXVertRender_h_included
#define SkeletonXVertRender_h_included
#pragma once

struct vertRender // T&B are not skinned, because in R2 skinning occurs always in hardware
{
    Fvector P;
    Fvector N;
    float u, v;
};

#endif //	SkeletonXVertRender_h_included
