///////////////////////////////////////////////////////////////
// MilitaryOutfit.h
// MilitaryOutfit - защитный костюм военного
///////////////////////////////////////////////////////////////

#pragma once

#include "customoutfit.h"

class CMilitaryOutfit : public CCustomOutfit
{
    RTTI_DECLARE_TYPEINFO(CMilitaryOutfit, CCustomOutfit);

private:
    typedef CCustomOutfit inherited;

public:
    CMilitaryOutfit();
    virtual ~CMilitaryOutfit();
};
