#include "stdafx.h"

#include "PHSimpleCalls.h"
#include "PhysicsShell.h"

void CPHCallOnStepCondition::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPHCallOnStepCondition>(
        "phcondition_callonstep", sol::no_constructor, sol::call_constructor, sol::constructors<CPHCallOnStepCondition()>(), "set_step", &CPHCallOnStepCondition::set_step,
        "set_steps_interval", &CPHCallOnStepCondition::set_steps_interval, "set_global_time_ms", sol::resolve<void(u32)>(&CPHCallOnStepCondition::set_global_time),
        "set_global_time_s", sol::resolve<void(float)>(&CPHCallOnStepCondition::set_global_time), "set_time_interval_ms",
        sol::resolve<void(u32)>(&CPHCallOnStepCondition::set_time_interval), "set_time_interval_s", sol::resolve<void(float)>(&CPHCallOnStepCondition::set_time_interval));
}

void CPHExpireOnStepCondition::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPHExpireOnStepCondition>("phcondition_expireonstep", sol::no_constructor, sol::call_constructor, sol::constructors<CPHExpireOnStepCondition()>(),
                                               sol::base_classes, xr::sol_bases<CPHExpireOnStepCondition>());
}

void CPHConstForceAction::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPHConstForceAction>("phaction_constforce", sol::no_constructor, sol::call_constructor,
                                          sol::constructors<CPHConstForceAction(CPhysicsShell*, const Fvector&)>());
}
