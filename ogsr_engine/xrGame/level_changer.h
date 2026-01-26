////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.h
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "..\xr_3da\feel_touch.h"
#include "game_graph_space.h"

class CLevelChanger : public CGameObject, public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(CLevelChanger, CGameObject, Feel::Touch);

private:
    using inherited = CGameObject;

private:
    GameGraph::_GRAPH_ID m_game_vertex_id{};
    u32 m_level_vertex_id{};
    Fvector m_position{};
    Fvector m_angles{};
    f32 m_entrance_time{};
    u8 m_SilentMode{};

    void update_actor_invitation();
    bool get_reject_pos(Fvector& p, Fvector& r);
    void ChangeLevel();

public:
    ~CLevelChanger() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    virtual void Center(Fvector& C) const;
    virtual float Radius() const;
    tmc::task<void> shedule_Update(u32 dt) override;
    virtual void feel_touch_new(CObject* O);
    virtual BOOL feel_touch_contact(CObject* O);

    virtual bool IsVisibleForZones() { return false; }

    void OnRender();
};

extern xr_vector<CLevelChanger*> g_lchangers;
