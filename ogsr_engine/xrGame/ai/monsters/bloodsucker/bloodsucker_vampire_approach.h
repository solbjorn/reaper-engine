#pragma once

#include "../state.h"

template <typename _Object>
class CStateBloodsuckerVampireApproach : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBloodsuckerVampireApproach<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateBloodsuckerVampireApproach(_Object* obj);
    ~CStateBloodsuckerVampireApproach() override;

    virtual void initialize();
    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "bloodsucker_vampire_approach_inline.h"
