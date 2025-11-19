#pragma once

class CBlender_gasmask_dudv : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_gasmask_dudv, IBlender);

public:
    virtual LPCSTR getComment() { return "Gasmask_dudv"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_gasmask_dudv();
    ~CBlender_gasmask_dudv() override;
};
