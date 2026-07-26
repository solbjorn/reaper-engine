////////////////////////////////////////////////////////////////////////////
//	Module 		: material_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Material manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xr_3da/GameMtlLib.h"

class CPHMovementControl;

class CMaterialManager final : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CMaterialManager);

private:
    CObject* m_object;
    CPHMovementControl* m_movement_control;
    ref_sound m_step_sound[4];

    f32 m_time_to_step{};
    u32 m_step_id{};
    u16 m_my_material_idx{GAMEMTL_NONE_IDX};
    bool m_run_mode{};

protected:
    u16 m_last_material_idx{};

public:
    explicit CMaterialManager(CObject* object, CPHMovementControl* movement_control);
    ~CMaterialManager() override;

    void Load(LPCSTR section);
    void reinit();
    void reload(LPCSTR);
    void set_run_mode(bool run_mode);
    void update(float time_delta, float volume, float step_time, bool standing);
    IC u16 last_material_idx() const;
    IC u16 self_material_idx() const;
    IC SGameMtlPair* get_current_pair();
};

#include "material_manager_inline.h"
