#pragma once

#include "../state.h"

template <typename _Object>
class CStatePsyDogPsyAttack : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStatePsyDogPsyAttack<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;

public:
    explicit CStatePsyDogPsyAttack(_Object* obj);
    ~CStatePsyDogPsyAttack() override = default;

    virtual void reselect_state();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "psy_dog_state_psy_attack_inline.h"
