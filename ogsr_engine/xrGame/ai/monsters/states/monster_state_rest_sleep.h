#pragma once

#include "../state.h"
#include "../../../ai_debug.h"

template <typename _Object>
class CStateMonsterRestSleep : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterRestSleep<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterRestSleep(_Object* obj);
    ~CStateMonsterRestSleep() override;

    void initialize() override;
    void execute() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "monster_state_rest_sleep_inline.h"
