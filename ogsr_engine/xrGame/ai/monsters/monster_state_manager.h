#pragma once

#include "state_manager.h"
#include "state.h"

template <typename _Object>
class XR_NOVTABLE CMonsterStateManager : public IStateManagerBase, public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CMonsterStateManager<_Object>, IStateManagerBase, CState<_Object>);

public:
    typedef CState<_Object> inherited;

    using inherited::add_state;
    using inherited::current_substate;
    using inherited::execute;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

    explicit CMonsterStateManager(_Object* obj) : inherited{obj} {}
    ~CMonsterStateManager() override = 0;

    virtual void reinit();
    virtual void update();
    virtual void force_script_state(EMonsterState state);
    virtual void execute_script_state();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object) = 0;

    virtual EMonsterState get_state_type();

    virtual bool check_control_start_conditions(ControlCom::EControlType type) { return inherited::check_control_start_conditions(type); }

protected:
    bool can_eat();
    bool check_state(u32 state_id);
};

template <typename _Object>
inline CMonsterStateManager<_Object>::~CMonsterStateManager() = default;

#include "monster_state_manager_inline.h"
