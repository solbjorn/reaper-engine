#include "stdafx.h"

#include "BoneProtections.h"

#include "..\Include/xrRender/Kinematics.h"
#include "..\Include/xrRender/KinematicsAnimated.h"

float SBoneProtections::getBoneProtection(s16 bone_id)
{
    storage_it it = m_bones_koeff.find(bone_id);
    if (it != m_bones_koeff.end())
        return it->second.koeff;
    else
        return m_default.koeff;
}

float SBoneProtections::getBoneArmour(s16 bone_id)
{
    storage_it it = m_bones_koeff.find(bone_id);
    if (it != m_bones_koeff.end())
        return it->second.armour;
    else
        return m_default.armour;
}

void SBoneProtections::reload(const shared_str& bone_sect, IKinematics* kinematics)
{
    VERIFY(kinematics);
    m_bones_koeff.clear();

    m_fHitFrac = READ_IF_EXISTS(pSettings, r_float, bone_sect, "hit_fraction", 0.1f);

    m_default.koeff = 1.0f;
    m_default.armour = 0.0f;

    CInifile::Sect& protections = pSettings->r_section(bone_sect);
    for (const auto& i : protections.Data)
    {
        string256 buffer;
        float Koeff = (float)atof(_GetItem(i.second.c_str(), 0, buffer));
        float Armour = (float)atof(_GetItem(i.second.c_str(), 1, buffer));

        BoneProtection BP;
        BP.koeff = Koeff;
        BP.armour = Armour;

        if (std::is_eq(xr_strcmp(i.first, "default")))
        {
            m_default = BP;
        }
        else
        {
            if (std::is_eq(xr_strcmp(i.first, "hit_fraction")))
                continue;

            s16 bone_id = kinematics->LL_BoneID(i.first);
            R_ASSERT2(gsl::narrow_cast<decltype(BI_NONE)>(bone_id) != BI_NONE, i.first.c_str());
            m_bones_koeff.try_emplace(bone_id, BP);
        }
    }
}
