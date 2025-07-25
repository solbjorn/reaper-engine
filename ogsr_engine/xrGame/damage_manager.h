////////////////////////////////////////////////////////////////////////////
//	Module 		: damage_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Damage manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CDamageManager : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CDamageManager);

protected:
    float m_default_hit_factor;
    float m_default_wound_factor;
    CObject* m_object;

public:
    CDamageManager();
    virtual ~CDamageManager();
    virtual DLL_Pure* _construct();
    virtual void reload(LPCSTR section, CInifile* ini);
    virtual void reload(LPCSTR section, LPCSTR sub_section, CInifile* ini);

    virtual void HitScale(const int bone_num, float& hit_scale, float& wound_scale, bool aim_bullet = false);

private:
    void load_section(LPCSTR section, CInifile* ini);
    // init default params
    void init_bones(LPCSTR section, CInifile* ini);
};
