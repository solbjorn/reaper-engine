#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "physicsshell.h"

static Fmatrix global_transform(CPhysicsElement* E)
{
    Fmatrix m;
    E->GetGlobalTransformDynamic(&m);
    return m;
}

static void freeze(CPhysicsShell* self)
{
    u16 max_elements = self->get_ElementsNumber();
    for (u16 i = 0; i < max_elements; ++i)
    {
        self->get_ElementByStoreOrder(i)->Fix();
    }
    self->SetIgnoreStatic();
}

static void unfreeze(CPhysicsShell* self)
{
    u16 max_elements = self->get_ElementsNumber();
    for (u16 i = 0; i < max_elements; ++i)
    {
        self->get_ElementByStoreOrder(i)->ReleaseFixed();
    }
    self->SetStatic();
}

static std::tuple<float, float> get_limits(CPhysicsJoint& self, int axis_num)
{
    float lo, hi;

    self.GetLimits(lo, hi, axis_num);

    return std::make_tuple(lo, hi);
}

static std::tuple<float, float> get_max_force_and_velocity(CPhysicsJoint& self, int axis_num)
{
    float force, velocity;

    self.GetMaxForceAndVelocity(force, velocity, axis_num);

    return std::make_tuple(force, velocity);
}

void CPhysicsShell::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPhysicsShell>(
        "physics_shell", sol::no_constructor, "apply_force", sol::resolve<void(float, float, float)>(&CPhysicsShell::applyForce), "get_element_by_bone_name",
        sol::resolve<CPhysicsElement*(LPCSTR)>(&CPhysicsShell::get_Element), "get_element_by_bone_id", sol::resolve<CPhysicsElement*(u16)>(&CPhysicsShell::get_Element),
        "get_element_by_order", &CPhysicsShell::get_ElementByStoreOrder, "get_elements_number", &CPhysicsShell::get_ElementsNumber, "get_joint_by_bone_name",
        sol::resolve<CPhysicsJoint*(LPCSTR)>(&CPhysicsShell::get_Joint), "get_joint_by_bone_id", sol::resolve<CPhysicsJoint*(u16)>(&CPhysicsShell::get_Joint), "get_joint_by_order",
        &CPhysicsShell::get_JointByStoreOrder, "get_joints_number", &CPhysicsShell::get_JointsNumber, "block_breaking", &CPhysicsShell::BlockBreaking, "unblock_breaking",
        &CPhysicsShell::UnblockBreaking, "is_breaking_blocked", &CPhysicsShell::IsBreakingBlocked, "is_breakable", &CPhysicsShell::isBreakable, "get_linear_vel",
        &CPhysicsShell::get_LinearVel, "get_angular_vel", &CPhysicsShell::get_AngularVel, "set_ignore_dynamic", &CPhysicsShell::SetIgnoreDynamic, "set_ignore_ragdoll",
        &CPhysicsShell::SetIgnoreRagDoll, "set_ignore_static", &CPhysicsShell::SetIgnoreStatic,

        // for anomaly mods
        "freeze", &freeze, "unfreeze", &unfreeze,

        "DisableCollision", &CPhysicsShell::DisableCollision, "EnableCollision", &CPhysicsShell::EnableCollision, "Disable", &CPhysicsShell::Disable, "Enable",
        &CPhysicsShell::Enable, "CollideAll", &CPhysicsShell::CollideAll, sol::base_classes, xr_sol_bases<CPhysicsShell>());
}

void CPhysicsElement::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPhysicsElement>(
        "physics_element", sol::no_constructor, "apply_force", sol::resolve<void(float, float, float)>(&CPhysicsElement::applyForce), "is_breakable", &CPhysicsElement::isBreakable,
        "get_linear_vel", &CPhysicsElement::get_LinearVel, "get_angular_vel", &CPhysicsElement::get_AngularVel, "get_mass", &CPhysicsElement::getMass, "get_density",
        &CPhysicsElement::getDensity, "get_volume", &CPhysicsElement::getVolume, "fix", &CPhysicsElement::Fix, "release_fixed", &CPhysicsElement::ReleaseFixed, "is_fixed",
        &CPhysicsElement::isFixed, "global_transform", &global_transform, sol::base_classes, xr_sol_bases<CPhysicsElement>());
}

void CPhysicsJoint::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPhysicsJoint>(
        "physics_joint", sol::no_constructor, "get_bone_id", &CPhysicsJoint::BoneID, "get_first_element", &CPhysicsJoint::PFirst_element, "get_stcond_element",
        &CPhysicsJoint::PSecond_element, "set_anchor_global", sol::resolve<void(const float, const float, const float)>(&CPhysicsJoint::SetAnchor), "set_anchor_vs_first_element",
        sol::resolve<void(const float, const float, const float)>(&CPhysicsJoint::SetAnchorVsFirstElement), "set_anchor_vs_second_element",
        sol::resolve<void(const float, const float, const float)>(&CPhysicsJoint::SetAnchorVsSecondElement), "get_axes_number", &CPhysicsJoint::GetAxesNumber,
        "set_axis_spring_dumping_factors", &CPhysicsJoint::SetAxisSDfactors, "set_joint_spring_dumping_factors", &CPhysicsJoint::SetJointSDfactors, "set_axis_dir_global",
        sol::resolve<void(const float, const float, const float, const int)>(&CPhysicsJoint::SetAxisDir), "set_axis_dir_vs_first_element",
        sol::resolve<void(const float, const float, const float, const int)>(&CPhysicsJoint::SetAxisDirVsFirstElement), "set_axis_dir_vs_second_element",
        sol::resolve<void(const float, const float, const float, const int)>(&CPhysicsJoint::SetAxisDirVsSecondElement), "set_limits", &CPhysicsJoint::SetLimits,
        "set_max_force_and_velocity", &CPhysicsJoint::SetForceAndVelocity, "get_max_force_and_velocity", &get_max_force_and_velocity, "get_axis_angle",
        &CPhysicsJoint::GetAxisAngle, "get_limits", &get_limits, "get_axis_dir", &CPhysicsJoint::GetAxisDirDynamic, "get_anchor", &CPhysicsJoint::GetAnchorDynamic, "is_breakable",
        &CPhysicsJoint::isBreakable);
}
