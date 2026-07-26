///////////////////////////////////////////////////////////////
// MilitaryOutfit.h
// MilitaryOutfit - защитный костюм военного
///////////////////////////////////////////////////////////////

#pragma once

#include "CustomOutfit.h"

class CMilitaryOutfit final : public CCustomOutfit
{
    RTTI_DECLARE_TYPEINFO(CMilitaryOutfit, CCustomOutfit);

private:
    typedef CCustomOutfit inherited;

public:
    CMilitaryOutfit();
    ~CMilitaryOutfit() override;
};
