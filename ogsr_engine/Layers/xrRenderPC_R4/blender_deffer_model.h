#pragma once

class CBlender_deffer_model : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_deffer_model, IBlenderXr);

public:
    xrP_INTEGER oAREF;
    xrP_BOOL oBlend;

public:
    [[nodiscard]] gsl::czstring getComment() override { return "LEVEL: deffer-model-flat"; }

    [[nodiscard]] BOOL canBeDetailed() override { return TRUE; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    CBlender_deffer_model();
    ~CBlender_deffer_model() override;

private:
    xrP_TOKEN oTessellation;
};
