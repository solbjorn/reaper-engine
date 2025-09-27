///////////////////////////////////////////////////////////////
// ScientificOutfit.h
// ScientificOutfit - защитный костюм ученого
///////////////////////////////////////////////////////////////

#pragma once

#include "customoutfit.h"

class CScientificOutfit : public CCustomOutfit
{
    RTTI_DECLARE_TYPEINFO(CScientificOutfit, CCustomOutfit);

private:
    typedef CCustomOutfit inherited;

public:
    CScientificOutfit();
    virtual ~CScientificOutfit();
};
