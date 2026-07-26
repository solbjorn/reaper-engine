#pragma once

#include "PHShell.h"

// Содержит информацию об целевой матрице анимации
class CPhysicsShellAnimatorBoneData final
{
    friend class CPhysicsShellAnimator;
    dJointID m_anim_fixed_dJointID;
    CPHElement* m_element;
};
