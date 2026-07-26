///////////////////////////////////////////////////////////////
// ScientificOutfit.h
// ScientificOutfit - защитный костюм ученого
///////////////////////////////////////////////////////////////

#pragma once

#include "CustomOutfit.h"

class CScientificOutfit final : public CCustomOutfit
{
    RTTI_DECLARE_TYPEINFO(CScientificOutfit, CCustomOutfit);

private:
    typedef CCustomOutfit inherited;

public:
    CScientificOutfit();
    ~CScientificOutfit() override;
};
