#pragma once

class CBlender_deffer_flat : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_deffer_flat, IBlenderXr);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "LEVEL: defer-base-normal"; }

    [[nodiscard]] BOOL canBeDetailed() override { return TRUE; }
    [[nodiscard]] BOOL canUseSteepParallax() override { return TRUE; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    CBlender_deffer_flat();
    ~CBlender_deffer_flat() override;

private:
    xrP_TOKEN oTessellation;
};
