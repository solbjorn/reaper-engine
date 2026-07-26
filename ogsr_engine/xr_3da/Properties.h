#pragma once

// Parameter/Property specifications

// *** FORMAT ***
// u32	type
// stringZ	name
// []		data

enum xrProperties : u32
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
};

struct xrP_INTEGER final
{
    int value{0};
    int min{0};
    int max{255};
};
static_assert(sizeof(xrP_INTEGER) == 12);

struct xrP_BOOL final
{
    BOOL value{FALSE};
};
static_assert(sizeof(xrP_BOOL) == 4);

struct xrP_TOKEN final
{
    struct Item final
    {
        u32 ID;
        string64 str;
    };

    u32 IDselected{0};
    u32 Count{0};
};
static_assert(sizeof(xrP_TOKEN) == 8);

// Base class
class XR_NOVTABLE CPropertyBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPropertyBase);

public:
    ~CPropertyBase() override = 0;

    virtual void Save(IWriter& fs) = 0;
    virtual void Load(IReader& fs, u16 version) = 0;

    virtual void SaveIni(CInifile* ini_file, gsl::czstring section) = 0;
    virtual void LoadIni(CInifile* ini_file, gsl::czstring section) = 0;
};

inline CPropertyBase::~CPropertyBase() = default;

// Writers
constexpr void xrPWRITE(IWriter& fs, u32 ID, LPCSTR name, LPCVOID data, u32 size)
{
    fs.w_u32(ID);
    fs.w_stringZ(name);

    if (data != nullptr && size > 0)
        fs.w(data, size);
}

constexpr void xrPWRITE_MARKER(IWriter& fs, LPCSTR name) { xrPWRITE(fs, xrPID_MARKER, name, nullptr, 0); }

#define xrPWRITE_PROP(FS, name, ID, data) xrPWRITE(fs, ID, name, &(data), sizeof(data))

// Readers
[[nodiscard]] constexpr u32 xrPREAD(IReader& fs)
{
    const auto T = fs.r_u32();
    fs.skip_stringZ();

    return T;
}

constexpr void xrPREAD_MARKER(IReader& fs) { XR_ASSERT(xrPREAD(fs) == xrPID_MARKER); }

#define xrPREAD_PROP(fs, ID, data) \
    { \
        XR_ASSERT(xrPREAD(fs) == ID); \
        fs.r(&(data), sizeof(data)); \
        switch (ID) \
        { \
        case xrPID_TOKEN: fs.advance(((xrP_TOKEN*)&(data))->Count * sizeof(xrP_TOKEN::Item)); break; \
        default: break; \
        } \
    } \
    XR_MACRO_END()
