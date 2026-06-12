#pragma once

#include "PHCommander.h"
#include "PHReqComparer.h"
#include "alife_space.h"
#include "script_export_space.h"

class CPhysicsShell;

class CPHCallOnStepCondition : public CPHCondition
{
    RTTI_DECLARE_TYPEINFO(CPHCallOnStepCondition, CPHCondition);

public:
    u64 m_step;

    CPHCallOnStepCondition();
    ~CPHCallOnStepCondition() override = default;

    [[nodiscard]] bool obsolete() const override;
    [[nodiscard]] bool is_true() override;
    IC void set_step(u64 step) { m_step = step; }
    void set_steps_interval(u64 steps);
    void set_time_interval(u32 time);
    void set_time_interval(float time);
    void set_global_time(float time);
    void set_global_time(u32 time);

private:
    IC bool time_out() const;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CPHCallOnStepCondition);
#undef script_type_list
#define script_type_list save_type_list(CPHCallOnStepCondition)

class CPHExpireOnStepCondition : public CPHCallOnStepCondition
{
    RTTI_DECLARE_TYPEINFO(CPHExpireOnStepCondition, CPHCallOnStepCondition);

public:
    CPHExpireOnStepCondition() = default;
    ~CPHExpireOnStepCondition() override = default;

    [[nodiscard]] bool is_true() override { return true; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CPHExpireOnStepCondition);

add_to_type_list(CPHExpireOnStepCondition);
#undef script_type_list
#define script_type_list save_type_list(CPHExpireOnStepCondition)

class CPHShellBasedAction : public CPHAction
{
    RTTI_DECLARE_TYPEINFO(CPHShellBasedAction, CPHAction);

protected:
    CPhysicsShell* m_shell;

public:
    explicit CPHShellBasedAction(CPhysicsShell* shell);
    ~CPHShellBasedAction() override = default;

    [[nodiscard]] virtual bool compare(const CPhysicsShell* shl) const { return shl == m_shell; }
    [[nodiscard]] bool obsolete() const override;
};

class CPHConstForceAction : public CPHShellBasedAction
{
    RTTI_DECLARE_TYPEINFO(CPHConstForceAction, CPHShellBasedAction);

public:
    Fvector m_force;

    explicit CPHConstForceAction(CPhysicsShell* shell, const Fvector& force);
    ~CPHConstForceAction() override = default;

    void run() override;

    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool compare(const CPhysicsShell* shl) const override { return CPHShellBasedAction::compare(shl); }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CPHConstForceAction);
#undef script_type_list
#define script_type_list save_type_list(CPHConstForceAction)

class CPHReqComparerHasShell : public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHReqComparerHasShell, CPHReqComparerV);

public:
    CPhysicsShell* m_shell;

    explicit CPHReqComparerHasShell(CPhysicsShell* shl);
    ~CPHReqComparerHasShell() override = default;

    [[nodiscard]] bool compare(const CPHConstForceAction* v) const override { return v->compare(m_shell); }
};
