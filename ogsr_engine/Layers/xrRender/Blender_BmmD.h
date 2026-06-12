// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_BmmD
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_BmmD

class CBlender_BmmD : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_BmmD, IBlenderXr);

public:
    string64 oT2_Name; // name of secondary texture

    string64 oR_Name; //. задел на будущее
    string64 oG_Name; //. задел на будущее
    string64 oB_Name; //. задел на будущее
    string64 oA_Name; //. задел на будущее

    [[nodiscard]] gsl::czstring getComment() override { return "LEVEL: Implicit**detail"; }
    [[nodiscard]] BOOL canBeDetailed() override { return TRUE; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    CBlender_BmmD();
    ~CBlender_BmmD() override;
};

#endif // AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_BmmD
