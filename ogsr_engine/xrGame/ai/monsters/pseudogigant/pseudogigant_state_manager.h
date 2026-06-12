#pragma once

#include "../monster_state_manager.h"

class CPseudoGigant;

class CStateManagerGigant : public CMonsterStateManager<CPseudoGigant>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerGigant, CMonsterStateManager<CPseudoGigant>);

private:
    typedef CMonsterStateManager<CPseudoGigant> inherited;
    using inherited::add_state;

public:
    explicit CStateManagerGigant(CPseudoGigant* monster);
    ~CStateManagerGigant() override = default;

    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};
