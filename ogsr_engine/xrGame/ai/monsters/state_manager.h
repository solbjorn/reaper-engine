#pragma once

#include "state_defs.h"
#include "control_com_defs.h"

class XR_NOVTABLE IStateManagerBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IStateManagerBase);

public:
    ~IStateManagerBase() override = 0;

    virtual void reinit() = 0;
    virtual void update() = 0;
    virtual void force_script_state(EMonsterState state) = 0;
    virtual void execute_script_state() = 0;
    virtual void critical_finalize() = 0;
    virtual void remove_links(CObject* O) = 0;
    [[nodiscard]] virtual EMonsterState get_state_type() = 0;

    [[nodiscard]] virtual bool check_control_start_conditions(ControlCom::EControlType type) = 0;
};

inline IStateManagerBase::~IStateManagerBase() = default;
