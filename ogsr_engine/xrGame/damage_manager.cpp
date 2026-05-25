////////////////////////////////////////////////////////////////////////////
//	Module 		: damage_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Damage manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "damage_manager.h"

#include "..\xr_3da\xr_object.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xr_3da/bone.h"

CDamageManager::CDamageManager() = default;
CDamageManager::~CDamageManager() = default;

DLL_Pure* CDamageManager::_construct()
{
    m_object = smart_cast<CObject*>(this);
    VERIFY(m_object);
    return (m_object);
}

void CDamageManager::reload(LPCSTR section, CInifile* ini)
{
    m_default_hit_factor = 1.f;
    m_default_wound_factor = 1.f;

    const bool section_exist = ini != nullptr && ini->section_exist(section);

    // прочитать дефолтные параметры
    if (section_exist && ini->line_exist(section, "default"))
    {
        const auto value = ini->r_string(section, "default");
        string32 buffer;

        auto res = scn::scan_value<f32>(std::string_view{_GetItem(value, 0, buffer)});
        R_ASSERT(res, res.error().msg());
        m_default_hit_factor = res->value();

        res = scn::scan_value<f32>(std::string_view{_GetItem(value, 2, buffer)});
        R_ASSERT(res, res.error().msg());
        m_default_wound_factor = res->value();
    }

    // инициализировать default параметрами
    init_bones();

    // записать поверху прописанные параметры
    if (section_exist)
        load_section(section, ini);
}

void CDamageManager::reload(LPCSTR section, LPCSTR line, CInifile* ini)
{
    if (ini && ini->section_exist(section) && ini->line_exist(section, line))
        reload(ini->r_string(section, line), ini);
    else
        reload(section, nullptr);
}

void CDamageManager::init_bones()
{
    IKinematics* kinematics = smart_cast<IKinematics*>(m_object->Visual());
    VERIFY(kinematics);
    for (u16 i = 0; i < kinematics->LL_BoneCount(); i++)
    {
        CBoneInstance& bone_instance = kinematics->LL_GetBoneInstance(i);
        bone_instance.set_param(0, m_default_hit_factor);
        bone_instance.set_param(1, 1.f);
        bone_instance.set_param(2, m_default_wound_factor);
    }
}

void CDamageManager::load_section(LPCSTR section, CInifile* ini)
{
    string32 buffer;
    IKinematics* kinematics = smart_cast<IKinematics*>(m_object->Visual());
    CInifile::Sect& damages = ini->r_section(section);

    for (const auto& i : damages.Data)
    {
        // read all except default line
        if (std::is_neq(xr_strcmp(i.first, "default")))
        {
            VERIFY(m_object);
            int bone = kinematics->LL_BoneID(i.first);
            ASSERT_FMT(bone != BI_NONE, "[%s]: bone '%s' not found in %s[%s] visual[%s]", __FUNCTION__, i.first.c_str(), m_object->cName().c_str(), section,
                       m_object->cNameVisual().c_str());

            CBoneInstance& bone_instance = kinematics->LL_GetBoneInstance(u16(bone));

            auto res = scn::scan_value<f32>(std::string_view{_GetItem(i.second.c_str(), 0, buffer)});
            R_ASSERT(res, res.error().msg());

            const auto param0 = res->value();
            bone_instance.set_param(0, param0);

            res = scn::scan_value<f32>(std::string_view{_GetItem(i.second.c_str(), 1, buffer)});
            R_ASSERT(res, res.error().msg());
            bone_instance.set_param(1, res->value());

            res = scn::scan_value<f32>(std::string_view{_GetItem(i.second.c_str(), 2, buffer)});
            R_ASSERT(res, res.error().msg());
            bone_instance.set_param(2, res->value());

            if (_GetItemCount(i.second.c_str()) >= 4)
            {
                res = scn::scan_value<f32>(std::string_view{_GetItem(i.second.c_str(), 3, buffer)});
                R_ASSERT(res, res.error().msg());
                bone_instance.set_param(3, res->value());
            }
            else
            {
                bone_instance.set_param(3, param0);
            }

            if (0 == bone && (fis_zero(bone_instance.get_param(0)) || fis_zero(bone_instance.get_param(2))))
            {
                string256 error_str;
                sprintf_s(error_str, "hit_scale and wound_scale for root bone cannot be zero. see section [%s]", section);
                R_ASSERT2(0, error_str);
            }
        }
    }
}

void CDamageManager::HitScale(const int element, float& hit_scale, float& wound_scale, bool aim_bullet)
{
    if (BI_NONE == u16(element) || !m_object->Visual()) // при выгрузке уровня были вылеты если визуала уже нет.
    {
        // считаем что параметры для BI_NONE заданы как 1.f
        hit_scale = 1.f * m_default_hit_factor;
        wound_scale = 1.f * m_default_wound_factor;
        return;
    }

    IKinematics* V = smart_cast<IKinematics*>(m_object->Visual());
    VERIFY(V);
    // get hit scale
    float scale;
    if (aim_bullet)
    {
        scale = V->LL_GetBoneInstance(u16(element)).get_param(3);
    }
    else
    {
        scale = V->LL_GetBoneInstance(u16(element)).get_param(0);
    }
    hit_scale = scale;

    // get wound scale
    scale = V->LL_GetBoneInstance(u16(element)).get_param(2);
    wound_scale = scale;
}
