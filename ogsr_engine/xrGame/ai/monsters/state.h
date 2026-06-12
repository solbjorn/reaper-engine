#pragma once

#include "state_defs.h"
#include "control_com_defs.h"

template <typename _Object>
class XR_NOVTABLE CState : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CState<_Object>);

public:
    typedef CState<_Object> CSState;

    explicit CState(_Object* obj, void* data = nullptr);
    ~CState() override;

    virtual void reinit();
    virtual void remove_links(CObject* object) = 0;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
    virtual void critical_finalize();

    virtual void reset();

    [[nodiscard]] virtual bool check_completion() { return false; }
    [[nodiscard]] virtual bool check_start_conditions() { return true; }

    virtual void reselect_state() {}
    virtual void check_force_state() {}

    CSState* get_state(u32 state_id);
    CSState* get_state_current();

    void fill_data_with(void* ptr_src, u32 size);

    [[nodiscard]] u32 time_started() { return time_state_started; }

    [[nodiscard]] virtual bool check_control_start_conditions(ControlCom::EControlType type);

protected:
    void select_state(u32 new_state_id);
    void add_state(u32 state_id, CSState* s);

    virtual void setup_substates() {}

    EMonsterState get_state_type();

    u32 current_substate;
    u32 prev_substate;

    u32 time_state_started;

    _Object* object;

    void* _data;

private:
    void free_mem();

    typedef xr_map<u32, CSState*> SubStates;
    SubStates substates;
    typedef typename xr_map<u32, CSState*>::iterator STATE_MAP_IT;
};

template <typename _Object>
class XR_NOVTABLE CStateMove : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMove<_Object>, CState<_Object>);

protected:
    using inherited = CState<_Object>;

public:
    explicit CStateMove(_Object* obj, void* data = nullptr) : inherited{obj, data} {}
    ~CStateMove() override = default;

    void initialize() override
    {
        inherited::initialize();
        this->object->path().prepare_builder();
    }
};

#include "state_inline.h"
