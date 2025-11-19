///////////////////////////////////////////////////////////////
// ExoOutfit.h
// ExoOutfit - защитный костюм с усилением
///////////////////////////////////////////////////////////////

#pragma once

#include "customoutfit.h"

class CExoOutfit : public CCustomOutfit
{
    RTTI_DECLARE_TYPEINFO(CExoOutfit, CCustomOutfit);

private:
    typedef CCustomOutfit inherited;

public:
    CExoOutfit();
    ~CExoOutfit() override;
};
