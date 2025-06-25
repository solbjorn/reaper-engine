#pragma once

class CBlender_luminance : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_luminance, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: luminance estimate"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_luminance();
    virtual ~CBlender_luminance();
};
