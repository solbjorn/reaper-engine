// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2

class CBlender_LmEbB : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_LmEbB, IBlenderXr);

private:
    string64 oT2_Name; // name of secondary texture

    xrP_BOOL oBlend;

public:
    [[nodiscard]] gsl::czstring getComment() override { return "LEVEL: lmap*(env^base)"; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    CBlender_LmEbB();
    ~CBlender_LmEbB() override;
};

#endif // AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2
