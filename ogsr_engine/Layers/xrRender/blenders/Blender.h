// Blender.h: interface for the IBlender class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_

#include "../../xr_3da/properties.h"
#include "Blender_Recorder.h"

class CBlender_DESC
{
public:
    CLASS_ID CLS;

    string128 cName;
    u32 cTime;

    u16 version;
    u16 pad;

    constexpr CBlender_DESC()
    {
        CLS = CLASS_ID(0);

        cName[0] = 0;
        cTime = 0;

        version = 0;
        pad = 0;
    }
};

class XR_NOVTABLE IBlender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IBlender);

public:
    friend class CBlender_Compile;

protected:
    CBlender_DESC description;

    xrP_INTEGER oPriority;
    xrP_BOOL oStrictSorting;

    string64 oT_Name;

public:
    [[nodiscard]] CBlender_DESC& getDescription() { return description; }
    [[nodiscard]] virtual gsl::czstring getName() { return description.cName; }
    [[nodiscard]] virtual gsl::czstring getComment() = 0;

    [[nodiscard]] virtual BOOL canBeDetailed() { return FALSE; }
    [[nodiscard]] virtual BOOL canUseSteepParallax() { return FALSE; }

    virtual void Compile(CBlender_Compile& C);

    IBlender();
    ~IBlender() override = 0;
};

inline IBlender::~IBlender() = default;

class IBlenderXr : public IBlender, public CPropertyBase
{
    RTTI_DECLARE_TYPEINFO(IBlenderXr, IBlender, CPropertyBase);

protected:
    static void WriteInteger(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_INTEGER v);
    static void WriteBool(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_BOOL v);
    static void WriteToken(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_TOKEN v);

    static void ReadInteger(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_INTEGER& v);
    static void ReadBool(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_BOOL& v);
    static void ReadToken(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_TOKEN& v);

public:
    ~IBlenderXr() override = default;

    [[nodiscard]] static IBlenderXr* Create(CLASS_ID cls);
    static void Destroy(IBlenderXr*& B);

    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;

    void SaveIni(CInifile* ini_file, gsl::czstring section) override;
    void LoadIni(CInifile* ini_file, gsl::czstring section) override;
};

#endif // AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
