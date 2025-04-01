#ifndef SkeletonXVertRender_h_included
#define SkeletonXVertRender_h_included

struct alignas(16) vertRender // T&B are not skinned, because in R2 skinning occurs always in hardware
{
    Fvector P;
    Fvector N;
    float u, v;
};
static_assert(sizeof(vertRender) == 32);

#endif //	SkeletonXVertRender_h_included
