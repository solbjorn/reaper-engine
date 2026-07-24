#include "stdafx.h"

#include "PhysicsShell.h"

#include "PHDynamicData.h"
#include "Physics.h"
#include "PHJoint.h"
#include "PHShell.h"
#include "PHJoint.h"
#include "PHJointDestroyInfo.h"
#include "PHSplitedShell.h"
#include "GameObject.h"
#include "PhysicsShellHolder.h"
#include "../Include/xrRender/Kinematics.h"

extern CPHWorld* ph_world;

CPhysicsShell::~CPhysicsShell()
{
    if (ph_world)
        ph_world->NetRelcase(this);
}

CPhysicsElement* P_create_Element()
{
    CPHElement* element = xr_new<CPHElement>();
    return element;
}

CPhysicsShell* P_create_Shell()
{
    CPhysicsShell* shell = xr_new<CPHShell>();
    return shell;
}

CPhysicsShell* P_create_splited_Shell()
{
    CPhysicsShell* shell = xr_new<CPHSplitedShell>();
    return shell;
}

CPhysicsJoint* P_create_Joint(CPhysicsJoint::enumType type, CPhysicsElement* first, CPhysicsElement* second)
{
    CPhysicsJoint* joint = xr_new<CPHJoint>(type, first, second);
    return joint;
}

CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, BONE_P_MAP* bone_map)
{
    IKinematics* pKinematics = smart_cast<IKinematics*>(obj->Visual());

    CPhysicsShell* pPhysicsShell = P_create_Shell();
#ifdef DEBUG
    pPhysicsShell->dbg_obj = smart_cast<CPhysicsShellHolder*>(obj);
#endif
    pPhysicsShell->build_FromKinematics(pKinematics, bone_map);

    pPhysicsShell->set_PhysicsRefObject(smart_cast<CPhysicsShellHolder*>(obj));
    pPhysicsShell->mXFORM.set(obj->XFORM());
    pPhysicsShell->Activate(not_active_state); //,
    // m_pPhysicsShell->SmoothElementsInertia(0.3f);
    pPhysicsShell->SetAirResistance(); // 0.0014f,1.5f

    return pPhysicsShell;
}

void fix_bones(LPCSTR fixed_bones, CPhysicsShell* shell)
{
    XR_ASSERT(fixed_bones != nullptr && shell != nullptr);
    IKinematics* pKinematics = XR_ASSERT_VAL(shell->PKinematics() != nullptr);

    for (int i = 0, count = _GetItemCount(fixed_bones); i < count; ++i)
    {
        string64 fixed_bone;
        std::ignore = _GetItem(fixed_bones, i, fixed_bone);

#ifdef DEBUG
        if (CPhysicsElement* E =
                shell->get_Element(XR_ASSERT_VAL(pKinematics->LL_BoneID(fixed_bone) != BI_NONE, "invalid fixed bone", pKinematics->getDebugName()));
            E != nullptr)
#else
        if (CPhysicsElement* E = shell->get_Element(XR_ASSERT_VAL(pKinematics->LL_BoneID(fixed_bone) != BI_NONE, "invalid fixed bone")); E != nullptr)
#endif
            E->Fix();
    }
}

CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, BONE_P_MAP* p_bone_map, LPCSTR fixed_bones)
{
    auto pKinematics = smart_cast<IKinematics*>(obj->Visual());
    CPhysicsShell* pPhysicsShell;

    if (fixed_bones != nullptr)
    {
        for (int i = 0, count = _GetItemCount(fixed_bones); i < count; ++i)
        {
            string64 fixed_bone;
            std::ignore = _GetItem(fixed_bones, i, fixed_bone);

            p_bone_map->try_emplace(XR_ASSERT_VAL(pKinematics->LL_BoneID(fixed_bone) != BI_NONE, "invalid fixed bone", obj->cName(), obj->cNameVisual()));
        }

        pPhysicsShell = P_build_Shell(obj, not_active_state, p_bone_map);
    }
    else
    {
        pPhysicsShell = P_build_Shell(obj, not_active_state);
    }

    if (p_bone_map->empty())
        return pPhysicsShell;

    pPhysicsShell->SetPrefereExactIntegration();

    for (auto& bone : *p_bone_map)
        XR_ASSERT_VAL(bone.second.element != nullptr, "fixed bone has no physics")->Fix();

    return pPhysicsShell;
}

CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, LPCSTR fixed_bones)
{
    xr_vector<u16> f_bones;

    if (fixed_bones != nullptr)
    {
        auto K = smart_cast<IKinematics*>(obj->Visual());

        for (int i = 0, count = _GetItemCount(fixed_bones); i < count; ++i)
        {
            string64 fixed_bone;
            std::ignore = _GetItem(fixed_bones, i, fixed_bone);

            f_bones.emplace_back(XR_ASSERT_VAL(K->LL_BoneID(fixed_bone) != BI_NONE, "invalid fixed bone", obj->cName(), obj->cNameVisual()));
        }
    }

    return P_build_Shell(obj, not_active_state, f_bones);
}

CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, U16Vec& fixed_bones)
{
    BONE_P_MAP bone_map;

    for (const auto id : fixed_bones)
        bone_map.try_emplace(id);

    CPhysicsShell* pPhysicsShell = P_build_Shell(obj, not_active_state, &bone_map);

    if (bone_map.empty())
        return pPhysicsShell;

    // fix bones
    pPhysicsShell->SetPrefereExactIntegration();

    for (auto& bone : bone_map)
    {
        if (CPhysicsElement* fixed_element = bone.second.element; fixed_element != nullptr)
            fixed_element->Fix();
    }

    return pPhysicsShell;
}

CPhysicsShell* P_build_SimpleShell(CGameObject* obj, float mass, bool not_active_state)
{
    CPhysicsShell* pPhysicsShell = P_create_Shell();
#ifdef DEBUG
    pPhysicsShell->dbg_obj = smart_cast<CPhysicsShellHolder*>(obj);
#endif

    Fobb obb;
    obj->Visual()->getVisData().box.get_CD(obb.m_translate, obb.m_halfsize);
    obb.m_rotate.identity();

    CPhysicsElement* E = XR_ASSERT_VAL(P_create_Element() != nullptr);
    E->add_Box(obb);

    pPhysicsShell->add_Element(E);
    pPhysicsShell->setMass(mass);
    pPhysicsShell->set_PhysicsRefObject(smart_cast<CPhysicsShellHolder*>(obj));

    if (obj->H_Parent() == nullptr)
        pPhysicsShell->Activate(obj->XFORM(), 0, obj->XFORM(), not_active_state);

    return pPhysicsShell;
}

void ApplySpawnIniToPhysicShell(CInifile* ini, CPhysicsShell* physics_shell, bool fixed)
{
    if (!ini)
        return;

    if (ini->section_exist("physics_common"))
    {
        fixed = fixed || (ini->line_exist("physics_common", "fixed_bones"));
        fix_bones(ini->r_string("physics_common", "fixed_bones"), physics_shell);
    }

    if (ini->section_exist("collide"))
    {
#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
        if ((ini->line_exist("collide", "ignore_static") && fixed) || (ini->line_exist("collide", "ignore_static") && ini->section_exist("animated_object")))
#else
        if (ini->line_exist("collide", "ignore_static") && fixed)
#endif
        {
            physics_shell->SetIgnoreStatic();
        }
        if (ini->line_exist("collide", "small_object"))
        {
            physics_shell->SetSmall();
        }
        if (ini->line_exist("collide", "ignore_small_objects"))
        {
            physics_shell->SetIgnoreSmall();
        }
        if (ini->line_exist("collide", "ignore_ragdoll"))
        {
            physics_shell->SetIgnoreRagDoll();
        }
        if (ini->line_exist("collide", "ignore_dynamic"))
        {
            physics_shell->SetIgnoreDynamic();
        }

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
        // If need, then show here that it is needed to ignore collisions with "animated_object"
        if (ini->line_exist("collide", "ignore_animated_objects"))
        {
            physics_shell->SetIgnoreAnimated();
        }
#endif
    }

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    // If next section is available then given "PhysicShell" is classified
    // as animated and we read options for his animation

    if (ini->section_exist("animated_object"))
    {
        // Show that given "PhysicShell" animated
        physics_shell->CreateShellAnimator();
    }
#endif
}

void get_box(CPhysicsShell* shell, const Fmatrix& form, Fvector& sz, Fvector& c)
{
    c.set(0, 0, 0);
    for (int i = 0; 3 > i; ++i)
    {
        float lo, hi;
        const Fvector& ax = cast_fv(((const float*)&form + i * 4));
        shell->get_Extensions(ax, 0, lo, hi);
        sz[i] = hi - lo;
        c.add(Fvector().mul(ax, (lo + hi) / 2));
    }
}
