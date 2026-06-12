#pragma once

class CBlender_deffer_aref : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_deffer_aref, IBlenderXr);

public:
    xrP_INTEGER oAREF;
    xrP_BOOL oBlend;
    bool lmapped;

public:
    [[nodiscard]] gsl::czstring getComment() override { return "LEVEL: defer-base-aref"; }

    [[nodiscard]] BOOL canBeDetailed() override { return TRUE; }
    [[nodiscard]] BOOL canUseSteepParallax() override { return TRUE; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    explicit CBlender_deffer_aref(bool _lmapped = false);
    ~CBlender_deffer_aref() override;
};
