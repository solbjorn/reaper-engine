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

    virtual LPCSTR getComment() { return "basic (simple)"; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_Screen_SET();
    ~CBlender_Screen_SET() override;
};

#endif // AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_
