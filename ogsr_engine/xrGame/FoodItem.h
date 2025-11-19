#pragma once

#include "eatable_item_object.h"

class CFoodItem : public CEatableItemObject
{
    RTTI_DECLARE_TYPEINFO(CFoodItem, CEatableItemObject);

public:
    CFoodItem();
    ~CFoodItem() override;
};
