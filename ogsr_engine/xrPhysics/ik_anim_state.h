#pragma once

class CBlend;
class motion_marks;
class IKinematicsAnimated;

class ik_anim_state
{
    const CBlend* current_blend{};
    bool is_step{};
    bool do_glue{true};
    bool is_idle{};
    bool is_blending{};

public:
    ik_anim_state() = default;

    void update(IKinematicsAnimated* K, const CBlend* b, u16 interval);
    IC bool step() const { return is_step; }
    IC bool glue() const { return do_glue; }
    IC bool idle() const { return is_idle; }
    IC bool blending() const { return is_blending; }
    IC bool auto_unstuck() const { return true /*|| idle() || blending()*/; }

    static bool time_step_begin(IKinematicsAnimated* K, const CBlend& B, u16 limb_id, float& time);
};
