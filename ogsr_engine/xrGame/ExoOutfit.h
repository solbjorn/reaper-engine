///////////////////////////////////////////////////////////////
// ExoOutfit.h
// ExoOutfit - защитный костюм с усилением
///////////////////////////////////////////////////////////////

#pragma once

#include "CustomOutfit.h"

class CExoOutfit final : public CCustomOutfit
{
    RTTI_DECLARE_TYPEINFO(CExoOutfit, CCustomOutfit);

private:
    typedef CCustomOutfit inherited;

public:
    CExoOutfit();
    ~CExoOutfit() override;
};
