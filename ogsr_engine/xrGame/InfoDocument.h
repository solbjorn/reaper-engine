///////////////////////////////////////////////////////////////
// InfoDocument.h
// InfoDocument - документ, содержащий сюжетную информацию
///////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item_object.h"
#include "InfoPortionDefs.h"
#include "script_export_space.h"

class CInfoDocument : public CInventoryItemObject
{
    RTTI_DECLARE_TYPEINFO(CInfoDocument, CInventoryItemObject);

private:
    typedef CInventoryItemObject inherited;

public:
    CInfoDocument();
    ~CInfoDocument() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    virtual void Load(LPCSTR section);
    tmc::task<void> net_Destroy() override;
    tmc::task<void> shedule_Update(u32 dt) override;
    tmc::task<void> UpdateCL() override;

    virtual void OnH_A_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);

    DECLARE_SCRIPT_REGISTER_FUNCTION();

protected:
    // индекс информации, содержащейся в документе
    xr_vector<xr_string> m_Info;
};
XR_SOL_BASE_CLASSES(CInfoDocument);

add_to_type_list(CInfoDocument);
#undef script_type_list
#define script_type_list save_type_list(CInfoDocument)
