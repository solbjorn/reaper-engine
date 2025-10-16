#pragma once

namespace sun
{
struct ray
{
    Fvector3 D;
    Fvector3 P;

    ray() = default;
    ray(Fvector3 const& _P, Fvector3 const& _D) : D{_D}, P{_P} {}
};

struct cascade
{
    ctx_id_t context_id{R__INVALID_CTX_ID};
    u32 cascade_ind;
    float size;
    float bias;

    xr_vector<ray> rays;
    CFrustum cull_frustum{};
    Fmatrix cull_xform{};
    Fvector3 cull_COP{};

    bool reset_chain{};
};
} // namespace sun
