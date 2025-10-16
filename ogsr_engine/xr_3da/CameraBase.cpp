// CameraBase.cpp: implementation of the CCameraBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CameraBase.h"
#include "igame_level.h"

CCameraBase::CCameraBase(CObject* p, u32 flags)
{
    SetParent(p);
    m_Flags.assign(flags);
}

CCameraBase::~CCameraBase() {}

void CCameraBase::Load(LPCSTR section)
{
    rot_speed = pSettings->r_fvector3(section, "rot_speed");

    lim_yaw = pSettings->r_fvector2(section, "lim_yaw");
    lim_pitch = pSettings->r_fvector2(section, "lim_pitch");

    bClampPitch = !fis_zero(lim_pitch[0]) || !fis_zero(lim_pitch[1]);
    bClampYaw = !fis_zero(lim_yaw[0]) || !fis_zero(lim_yaw[1]);

    if (bClampPitch)
        pitch = (lim_pitch[0] + lim_pitch[1]) * 0.5f;
    if (bClampYaw)
        yaw = (lim_yaw[0] + lim_yaw[1]) * 0.5f;
}

SCamEffectorInfo::SCamEffectorInfo() = default;

SCamEffectorInfo::SCamEffectorInfo(const SCamEffectorInfo&) = default;
SCamEffectorInfo::SCamEffectorInfo(SCamEffectorInfo&&) = default;

SCamEffectorInfo& SCamEffectorInfo::operator=(const SCamEffectorInfo&) = default;
SCamEffectorInfo& SCamEffectorInfo::operator=(SCamEffectorInfo&&) = default;
