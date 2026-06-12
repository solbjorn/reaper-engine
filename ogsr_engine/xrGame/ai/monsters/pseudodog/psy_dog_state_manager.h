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

    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};
