#pragma once

#include "ik_calculate_state.h"

class CIKLimb;

struct SCalculateData
{
    float const* m_angles{};
    CIKLimb* m_limb{};
    Fmatrix const* m_obj{};

    calculate_state state{};
    Fvector cl_shift{};

    float l{};
    float a{};
    bool apply{};
    bool do_collide{};

public:
    SCalculateData() = default;
    SCalculateData(CIKLimb& l, const Fmatrix& o);

public:
    IC Fmatrix& goal(Fmatrix& g) const;
};

// #define IK_DBG_STATE_SEQUENCE
#ifdef IK_DBG_STATE_SEQUENCE
extern u32 sdbg_state_sequence_number;

#include "ik_dbg_matrix.h"
#endif
