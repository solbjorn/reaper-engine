#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterHitObject : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterHitObject<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    xr_vector<CObject*> m_nearest_objects;
    CPhysicsShellHolder* target;
    bool m_hitted;

public:
    explicit CStateMonsterHitObject(_Object* obj) : inherited{obj} {}
    ~CStateMonsterHitObject() override = default;

    virtual void initialize();
    virtual void execute();
    virtual bool check_start_conditions();
    virtual bool check_completion();
};

#include "state_hit_object_inline.h"
