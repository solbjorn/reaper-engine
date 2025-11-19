#pragma once

template <typename _Object>
class CStateMonsterDrag : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterDrag<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    Fvector m_cover_position;
    u32 m_cover_vertex_id;

    bool m_failed;
    Fvector m_corpse_start_position;

public:
    explicit CStateMonsterDrag(_Object* obj);
    ~CStateMonsterDrag() override;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
};

#include "monster_state_eat_drag_inline.h"
