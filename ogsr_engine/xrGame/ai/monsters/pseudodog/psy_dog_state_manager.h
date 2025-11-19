#pragma once

#include "pseudodog_state_manager.h"

class CStateManagerPsyDog : public CStateManagerPseudodog
{
    RTTI_DECLARE_TYPEINFO(CStateManagerPsyDog, CStateManagerPseudodog);

private:
    typedef CStateManagerPseudodog inherited;

public:
    explicit CStateManagerPsyDog(CAI_PseudoDog* monster);
    ~CStateManagerPsyDog() override = default;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
