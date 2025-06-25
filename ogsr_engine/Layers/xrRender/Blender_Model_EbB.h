// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_21)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_21

class CBlender_Model_EbB : public IBlenderXr
{
    RTTI_DECLARE_TYPEINFO(CBlender_Model_EbB, IBlenderXr);

private:
    string64 oT2_Name; // name of secondary texture

    xrP_BOOL oBlend;

public:
    virtual LPCSTR getComment() { return "MODEL: env^base"; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_Model_EbB();
    virtual ~CBlender_Model_EbB();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2)
