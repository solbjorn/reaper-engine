#pragma once

#include "../state.h"

template <typename _Object>
class CStateChimeraThreatenRoar : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateChimeraThreatenRoar<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

public:
    explicit CStateChimeraThreatenRoar(_Object* obj) : inherited{obj} {}
    ~CStateChimeraThreatenRoar() override = default;

    virtual void initialize();
    virtual void execute();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "chimera_state_threaten_roar_inline.h"
