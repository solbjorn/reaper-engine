#ifndef dx10MSAABlender_included
#define dx10MSAABlender_included

class CBlender_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_msaa, IBlender);

public:
    ~CBlender_msaa() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: DX10 msaa blender"; }

    void Compile(CBlender_Compile& C) override;
};

#endif //	dx10RainBlender_included
