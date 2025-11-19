#pragma once

class CBlender_accum_point : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_point, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate point light"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_accum_point();
    ~CBlender_accum_point() override;
};

class CBlender_accum_point_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_point_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate point light msaa"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_accum_point_msaa();
    ~CBlender_accum_point_msaa() override;

    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    LPCSTR Name;
    LPCSTR Definition;
};
