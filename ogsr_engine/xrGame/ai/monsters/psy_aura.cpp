#include "stdafx.h"

#include "psy_aura.h"

#include "BaseMonster/base_monster.h"

CPsyAura::CPsyAura() = default;
CPsyAura::~CPsyAura() = default;

tmc::task<void> CPsyAura::schedule_update()
{
    co_await inherited::schedule_update();

    if (is_active())
    {
        feel_touch_update(m_object->Position(), m_radius);
        process_objects_in_aura();
    }
}
