#include "stdafx.h"

#include "zone_effector.h"

#include "level.h"
#include "clsid_game.h"
#include "..\xr_3da\xr_object.h"
#include "..\xr_3da\cameramanager.h"
#include "actor.h"
#include "actoreffector.h"
#include "postprocessanimator.h"

CZoneEffector::CZoneEffector() = default;
CZoneEffector::~CZoneEffector() { Stop(); }

void CZoneEffector::Load(LPCSTR section)
{
    m_pp_fname._set(READ_IF_EXISTS(pSettings, r_string, section, "ppe_file", READ_IF_EXISTS(pSettings, r_string, section, "pp_eff_name", nullptr)));
    m_cam_fname._set(READ_IF_EXISTS(pSettings, r_string, section, "cam_file", nullptr));

    r_min_perc = pSettings->r_float(section, "radius_min");
    r_max_perc = pSettings->r_float(section, "radius_max");
    VERIFY(r_min_perc <= r_max_perc);
}

void CZoneEffector::Activate()
{
    m_pActor = smart_cast<CActor*>(Level().CurrentEntity());
    if (m_pActor == nullptr)
        return;

    if (m_pp_effector == nullptr && !m_pp_fname.empty())
    {
        m_pp_effector = xr_new<CPostprocessAnimatorLerp>();
        m_pp_effector->SetType(EEffectorPPType{hash_64(reinterpret_cast<uintptr_t>(this), 32)});
        m_pp_effector->SetCyclic(true);
        m_pp_effector->SetFactorFunc(CallMe::fromMethod<&CZoneEffector::GetFactor>(this));
        m_pp_effector->Load(m_pp_fname.c_str());
        m_pActor->Cameras().AddPPEffector(m_pp_effector);
    }

    if (m_cam_effector == nullptr && !m_cam_fname.empty())
    {
        m_cam_effector = xr_new<CAnimatorCamLerpEffector>();
        m_cam_effector->SetType(effZone);
        m_cam_effector->SetCyclic(true);
        m_cam_effector->SetFactorFunc(CallMe::fromMethod<&CZoneEffector::GetFactor>(this));
        m_cam_effector->Start(m_cam_fname.c_str());
        m_pActor->Cameras().AddCamEffector(m_cam_effector);
    }
}

void CZoneEffector::Stop()
{
    if (m_pp_effector != nullptr)
    {
        m_pActor->Cameras().RemovePPEffector(EEffectorPPType{hash_64(reinterpret_cast<uintptr_t>(this), 32)});
        m_pp_effector = nullptr;
    }

    if (m_cam_effector != nullptr)
    {
        m_pActor->Cameras().RemoveCamEffector(effZone);
        m_cam_effector = nullptr;
    }

    m_pActor = nullptr;
}

void CZoneEffector::Update(float dist)
{
    float min_r = radius * r_min_perc;
    float max_r = radius * r_max_perc;

    bool camera_on_actor = (Level().CurrentEntity() && (Level().CurrentEntity()->CLS_ID == CLSID_OBJECT_ACTOR));

    if (dist > max_r || !camera_on_actor || (m_pActor != nullptr && !m_pActor->g_Alive()))
        Stop();
    else if (dist < max_r && camera_on_actor)
        Activate();

    if (m_pp_effector != nullptr || m_cam_effector != nullptr)
    {
        m_factor = (max_r - dist) / (max_r - min_r);
        clamp(m_factor, 0.01f, 1.0f);
    }
}

float CZoneEffector::GetFactor() { return m_factor; }

void CZoneEffector::SetRadius(float r) { radius = r; }
