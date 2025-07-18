#pragma once
#pragma pack(push, 4)

// Parameter/Property specifications

// *** FORMAT ***
// u32	type
// stringZ	name
// []		data

enum xrProperties
{
    xrPID_MARKER = 0,
    xrPID_MATRIX, // really only name(stringZ) is written into stream
    xrPID_CONSTANT, // really only name(stringZ) is written into stream
    xrPID_TEXTURE, // really only name(stringZ) is written into stream
    xrPID_INTEGER,
    xrPID_FLOAT,
    xrPID_BOOL,
    xrPID_TOKEN,
    xrPID_CLSID,
    xrPID_OBJECT, // really only name(stringZ) is written into stream
    xrPID_STRING, // really only name(stringZ) is written into stream
    xrPID_MARKER_TEMPLATE,
    xrPID_FORCEDWORD = u32(-1)
};

struct xrP_INTEGER
{
    int value;
    int min;
    int max;

    xrP_INTEGER() : value(0), min(0), max(255) {}
};

struct xrP_BOOL
{
    BOOL value;

    xrP_BOOL() : value(FALSE) {}
};

struct xrP_TOKEN
{
    struct Item
    {
        u32 ID;
        string64 str;
    };

    u32 IDselected;
    u32 Count;

    xrP_TOKEN() : IDselected(0), Count(0) {}
};

// Base class
class CPropertyBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPropertyBase);

public:
    virtual void Save(IWriter& fs) = 0;
    virtual void Load(IReader& fs, u16 version) = 0;

    virtual void SaveIni(CInifile* ini_file, LPCSTR section) = 0;
    virtual void LoadIni(CInifile* ini_file, LPCSTR section) = 0;
};

// Writers
IC void xrPWRITE(IWriter& fs, u32 ID, LPCSTR name, LPCVOID data, u32 size)
{
    fs.w_u32(ID);
    fs.w_stringZ(name);
    if (data && size)
        fs.w(data, size);
}
IC void xrPWRITE_MARKER(IWriter& fs, LPCSTR name) { xrPWRITE(fs, xrPID_MARKER, name, 0, 0); }

#define xrPWRITE_PROP(FS, name, ID, data) \
    { \
        xrPWRITE(fs, ID, name, &data, sizeof(data)); \
    }

// Readers
IC u32 xrPREAD(IReader& fs)
{
    u32 T = fs.r_u32();
    fs.skip_stringZ();
    return T;
}
IC void xrPREAD_MARKER(IReader& fs) { R_ASSERT(xrPID_MARKER == xrPREAD(fs)); }

#define xrPREAD_PROP(fs, ID, data) \
    { \
        R_ASSERT(ID == xrPREAD(fs)); \
        fs.r(&data, sizeof(data)); \
        switch (ID) \
        { \
        case xrPID_TOKEN: fs.advance(((xrP_TOKEN*)&data)->Count * sizeof(xrP_TOKEN::Item)); break; \
        }; \
    }

#pragma pack(pop)
