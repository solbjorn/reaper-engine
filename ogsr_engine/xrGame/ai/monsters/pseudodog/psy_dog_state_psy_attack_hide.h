#pragma once

template <typename _Object>
class CStatePsyDogHide : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStatePsyDogHide<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;

    struct
    {
        Fvector position;
        u32 node;
    } target;

public:
    explicit CStatePsyDogHide(_Object* obj) : inherited{obj} {}
    ~CStatePsyDogHide() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
    virtual bool check_start_conditions();

private:
    void select_target_point();
};

#include "psy_dog_state_psy_attack_hide_inline.h"
