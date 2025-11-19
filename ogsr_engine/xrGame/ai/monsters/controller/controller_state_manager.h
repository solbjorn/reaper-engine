#pragma once

#include "../monster_state_manager.h"

class CController;

class CStateManagerController : public CMonsterStateManager<CController>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerController, CMonsterStateManager<CController>);

private:
    typedef CMonsterStateManager<CController> inherited;

public:
    explicit CStateManagerController(CController* obj);
    ~CStateManagerController() override;

    virtual void reinit();
    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
    virtual bool check_control_start_conditions(ControlCom::EControlType type);
};
