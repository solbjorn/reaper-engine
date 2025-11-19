#ifndef dx10RainBlender_included
#define dx10RainBlender_included

class CBlender_rain : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_rain, IBlender);

public:
    ~CBlender_rain() override = default;

    virtual LPCSTR getComment() { return "INTERNAL: DX10 rain blender"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_rain_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_rain_msaa, IBlender);

public:
    LPCSTR Name{};
    LPCSTR Definition{};

    CBlender_rain_msaa() = default;
    ~CBlender_rain_msaa() override = default;

    virtual LPCSTR getComment() { return "INTERNAL: DX10 MSAA rain blender"; }

    virtual void Compile(CBlender_Compile& C);
    virtual void SetDefine(LPCSTR Name, LPCSTR Definition);
};

#endif //	dx10RainBlender_included
