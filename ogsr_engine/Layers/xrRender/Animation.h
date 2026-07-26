#pragma once

#include "KinematicAnimatedDefs.h"

namespace animation
{
enum mix_type
{
    lerp = 0,
    add,
};

struct channal_rule final
{
    mix_type intern_;
    mix_type extern_;
};

struct channel_def final
{
    float factor;
    channal_rule rule;
};

class channels final
{
public:
    static constexpr u32 max{MAX_CHANNELS};

private:
    static const channal_rule rules[max];

    float factors[max];

public:
    channels();

    void init();
    void set_factor(u16 channel, float factor);

public:
    IC const channal_rule& rule(u16 channel) { return rules[channel]; }

    IC void get_def(u16 channel, channel_def& def)
    {
        def.rule = rules[XR_ASSERT_VAL(channel < max)];
        def.factor = factors[channel];
    }
};
} // namespace animation
