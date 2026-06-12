// Blender_Screen_SET.h: interface for the Blender_Screen_SET class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_
#define AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_

class CBlender_Screen_SET : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_Screen_SET, IBlenderXr);

public:
    xrP_TOKEN oBlend;
    xrP_INTEGER oAREF;
    xrP_BOOL oZTest;
    xrP_BOOL oZWrite;
    xrP_BOOL oLighting;
    xrP_BOOL oFog;
    xrP_BOOL oClamp;

    [[nodiscard]] gsl::czstring getComment() override { return "basic (simple)"; }

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;

    void Compile(CBlender_Compile& C) override;

    CBlender_Screen_SET();
    ~CBlender_Screen_SET() override;
};

#endif // AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_
