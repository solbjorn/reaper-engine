#pragma once

class CBlender_gasmask_drops : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_gasmask_drops, IBlender);

public:
    virtual LPCSTR getComment() { return "Gasmask_drops"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_gasmask_drops();
    ~CBlender_gasmask_drops() override;
};
