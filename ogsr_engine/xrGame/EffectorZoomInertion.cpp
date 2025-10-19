// EffectorZoomInertion.cpp: инерция(покачивания) оружия в режиме
//							 приближения
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "EffectorZoomInertion.h"

#include "Actor.h"
#include "ActorEffector.h"

namespace
{
constexpr const char* EFFECTOR_ZOOM_SECTION = "zoom_inertion_effector";

bool external_zoom_osc{}; // alpet: флажок внешнего рассчета колебаний прицела (из скриптов).

void switch_zoom_osc(bool bExternal) { external_zoom_osc = bExternal; }

CEffectorZoomInertion* FindEffectorZoomInertion()
{
    CActor* actor = Actor();
    if (!actor)
        return nullptr;

    CEffectorCam* eff = actor->Cameras().GetCamEffector(eCEZoom);
    if (eff)
        return (CEffectorZoomInertion*)(eff);

    return nullptr;
}
} // namespace

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorZoomInertion::CEffectorZoomInertion() : CEffectorCam(eCEZoom, 100000.f)
{
    Load();
    m_dwTimePassed = 0;
}

CEffectorZoomInertion::~CEffectorZoomInertion() {}

void CEffectorZoomInertion::LoadParams(LPCSTR Section, LPCSTR Prefix)
{
    string256 full_name;
    m_fCameraMoveEpsilon = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "camera_move_epsilon"),
                                          pSettings->r_float(EFFECTOR_ZOOM_SECTION, "camera_move_epsilon"));
    m_fDispMin = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "disp_min"), pSettings->r_float(EFFECTOR_ZOOM_SECTION, "disp_min"));
    m_fSpeedMin = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "speed_min"), pSettings->r_float(EFFECTOR_ZOOM_SECTION, "speed_min"));
    m_fZoomAimingDispK = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "zoom_aim_disp_k"),
                                        pSettings->r_float(EFFECTOR_ZOOM_SECTION, "zoom_aim_disp_k"));
    m_fZoomAimingSpeedK = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "zoom_aim_speed_k"),
                                         pSettings->r_float(EFFECTOR_ZOOM_SECTION, "zoom_aim_speed_k"));
    m_dwDeltaTime = READ_IF_EXISTS(pSettings, r_u32, Section, strconcat(sizeof(full_name), full_name, Prefix, "delta_time"), pSettings->r_u32(EFFECTOR_ZOOM_SECTION, "delta_time"));
}

void CEffectorZoomInertion::Load()
{
    LoadParams(EFFECTOR_ZOOM_SECTION, "");

    m_dwTimePassed = 0;

    m_fFloatSpeed = m_fSpeedMin;
    m_fDispRadius = m_fDispMin;

    m_fEpsilon = 2 * m_fFloatSpeed;

    m_vTargetVel.set(0.f, 0.f, 0.f);
    m_vCurrentPoint.set(0.f, 0.f, 0.f);
    m_vTargetPoint.set(0.f, 0.f, 0.f);
    m_vLastPoint.set(0.f, 0.f, 0.f);
}

void CEffectorZoomInertion::Init(CWeaponMagazined* pWeapon)
{
    if (!pWeapon)
        return;

    LoadParams(*pWeapon->cNameSect(), "ezi_");
}

void CEffectorZoomInertion::SetParams(float disp)
{
    float old_disp = m_fDispRadius;

    m_fDispRadius = disp * m_fZoomAimingDispK;
    if (m_fDispRadius < m_fDispMin)
        m_fDispRadius = m_fDispMin;

    m_fFloatSpeed = disp * m_fZoomAimingSpeedK;
    if (m_fFloatSpeed < m_fSpeedMin)
        m_fFloatSpeed = m_fSpeedMin;

    // для того, чтоб сразу прошел пересчет направления
    // движения прицела
    if (!fis_zero(old_disp - m_fDispRadius, EPS))
        m_fEpsilon = 2 * m_fDispRadius;
}

void CEffectorZoomInertion::CalcNextPoint()
{
    //	m_fEpsilon = 2*m_fFloatSpeed;

    float half_disp_radius = m_fDispRadius / 2.f;
    m_vTargetPoint.x = ::Random.randF(-half_disp_radius, half_disp_radius);
    m_vTargetPoint.y = ::Random.randF(-half_disp_radius, half_disp_radius);

    m_vTargetVel.sub(m_vTargetPoint, m_vLastPoint);
}

BOOL CEffectorZoomInertion::ProcessCam(SCamEffectorInfo& info)
{
    bool camera_moved = false;

    // определяем двигал ли прицелом актер
    if (!info.d.similar(m_vOldCameraDir, m_fCameraMoveEpsilon))
        camera_moved = true;

    /*
    Fvector dir;
    dir.sub(m_vCurrentPoint,m_vTargetPoint);*/

    ///	if(dir.magnitude()<m_fEpsilon || m_dwTimePassed>m_dwDeltaTime)
    //	if (m_dwTimePassed>m_dwDeltaTime)
    if (m_dwTimePassed == 0)
    {
        m_vLastPoint.set(m_vCurrentPoint);
        CalcNextPoint();
    }
    else
    {
        while (m_dwTimePassed > m_dwDeltaTime)
        {
            m_dwTimePassed -= m_dwDeltaTime;

            m_vLastPoint.set(m_vTargetPoint);
            CalcNextPoint();
        }
    }

    m_vCurrentPoint.lerp(m_vLastPoint, m_vTargetPoint, float(m_dwTimePassed) / m_dwDeltaTime);

    m_vOldCameraDir = info.d;

    if (!camera_moved)
        info.d.add(m_vCurrentPoint);

    m_dwTimePassed += Device.dwTimeDelta;

    return TRUE;
}

static Fvector get_current_point(CEffectorZoomInertion* E) { return E->m_vCurrentPoint; }
static Fvector get_last_point(CEffectorZoomInertion* E) { return E->m_vLastPoint; }
static Fvector get_target_point(CEffectorZoomInertion* E) { return E->m_vTargetPoint; }

static void set_current_point(CEffectorZoomInertion* E, const Fvector src) { E->m_vCurrentPoint.set(src); }
static void set_last_point(CEffectorZoomInertion* E, const Fvector src) { E->m_vLastPoint.set(src); }
static void set_target_point(CEffectorZoomInertion* E, const Fvector src) { E->m_vTargetPoint.set(src); }

void CEffectorZoomInertion::script_register(sol::state_view& lua)
{
    lua.new_usertype<CEffectorZoomInertion>("CEffectorZoomInertion", sol::no_constructor, "float_speed", &CEffectorZoomInertion::m_fFloatSpeed, "disp_radius",
                                            &CEffectorZoomInertion::m_fDispRadius, "epsilon", &CEffectorZoomInertion::m_fEpsilon, "current_point",
                                            sol::property(&get_current_point, &set_current_point), "last_point", sol::property(&get_last_point, &set_last_point), "target_point",
                                            sol::property(&get_target_point, &set_target_point), "target_vel", &CEffectorZoomInertion::m_vTargetVel, "time_passed",
                                            &CEffectorZoomInertion::m_dwTimePassed,
                                            // settings for real-time modify
                                            "camera_move_epsilon", &CEffectorZoomInertion::m_fCameraMoveEpsilon, "disp_min", &CEffectorZoomInertion::m_fDispMin, "speed_min",
                                            &CEffectorZoomInertion::m_fSpeedMin, "zoom_aim_disp_k", &CEffectorZoomInertion::m_fZoomAimingDispK, "zoom_aim_speed_k",
                                            &CEffectorZoomInertion::m_fZoomAimingSpeedK, "delta_time", &CEffectorZoomInertion::m_dwDeltaTime);

    lua.set("find_effector_zi", &FindEffectorZoomInertion, "switch_zoom_osc", &switch_zoom_osc);
}
