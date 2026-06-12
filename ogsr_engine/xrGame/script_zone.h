////////////////////////////////////////////////////////////////////////////
//	Module 		: script_zone.h
//	Created 	: 10.10.2003
//  Modified 	: 10.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script zone object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "space_restrictor.h"
#include "script_export_space.h"

class CScriptGameObject;

class CScriptZone : public CSpaceRestrictor
{
    RTTI_DECLARE_TYPEINFO(CScriptZone, CSpaceRestrictor);

public:
    typedef CSpaceRestrictor inherited;

    CScriptZone();
    ~CScriptZone() override;

    void reinit() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Relcase(CObject* O) override;
    tmc::task<void> shedule_Update(u32 dt) override;
    void feel_touch_new(CObject* O) override;
    void feel_touch_delete(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;
    bool active_contact(u16 id) const;
    [[nodiscard]] bool IsVisibleForZones() override { return false; }
    [[nodiscard]] bool register_schedule() const override { return true; }

#ifdef DEBUG
    virtual void OnRender();
#endif

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CScriptZone);

add_to_type_list(CScriptZone);
#undef script_type_list
#define script_type_list save_type_list(CScriptZone)
