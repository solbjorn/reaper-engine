#pragma once

class ik_goal_matrix
{
public:
    enum e_collide_state
    {
        cl_free,
        cl_rotational,
        cl_translational,
        cl_mixed,
        cl_aligned,
        cl_undefined
    };

private:
    Fmatrix m;
    e_collide_state cl_state;

public:
    ik_goal_matrix() : m{Fidentity}, cl_state{cl_undefined} {}

    IC const Fmatrix& get() const { return m; }

    IC void set(const Fmatrix& m_, e_collide_state cl)
    {
        m.set(m_);
        cl_state = cl;
    }

    IC e_collide_state collide_state() const { return cl_state; }
};

struct calculate_state
{
    u32 calc_time{};
    u32 unstuck_time{std::numeric_limits<u32>::max()};
    ik_goal_matrix goal;
    ik_goal_matrix blend_to;
    Fmatrix anim_pos{Fidentity};
    ik_goal_matrix collide_pos;
    Fmatrix b2tob3{Fidentity};
    Fvector pick{0.0f, -1.0f, 0.0f};
    float speed_blend_l{};
    float speed_blend_a{};
    bool foot_step{};
    bool idle{};
    bool blending{};
    u16 ref_bone{std::numeric_limits<u16>::max()};

#ifdef DEBUG
    int count{-1};
#endif
};
