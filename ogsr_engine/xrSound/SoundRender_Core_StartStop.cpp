#include "stdafx.h"

#include "SoundRender_Core.h"

#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"
#include "SoundRender_Source.h"

tmc::task<void> CSoundRender_Core::i_start(CSoundRender_Emitter* E) const
{
    R_ASSERT(E);

    // Search lowest-priority target
    float Ptest = E->priority();
    float Ptarget = flt_max;

    CSoundRender_Target* T = nullptr;

    // stopped targets will have -1 priority
    for (auto* Ttest : s_targets)
    {
        if (Ttest->priority < Ptarget)
        {
            T = Ttest;
            Ptarget = Ttest->priority;
        }
    }

    // Stop currently playing
    if (CSoundRender_Emitter* emitter = T->get_emitter())
        co_await emitter->cancel();

    // Associate
    E->target = T;
    E->target->start(E);

    T->priority = Ptest;

    if (E->target->get_emitter() && !E->target->get_emitter()->b2D)
    {
        float dist = SoundRender->listener_position().distance_to(E->target->get_emitter()->p_source.position);
        if (dist > E->target->get_emitter()->p_source.max_distance)
            co_await E->target->get_emitter()->cancel();
    }
}

bool CSoundRender_Core::i_allow_play(const CSoundRender_Emitter* E) const
{
    // Search available target
    const float Ptest = E->priority();
    return std::ranges::any_of(s_targets, [Ptest](const CSoundRender_Target* target) { return target->get_priority() < Ptest; });
}
