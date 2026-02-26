#pragma once

namespace xr
{
namespace fsgame
{
namespace formats
{
constexpr inline std::array<std::string_view, 17> texture{".dds", ".exr", ".ktx", ".ktx2", ".qoi", ".png", ".tga", ".jpg", ".bmp",
                                                          ".gif", ".psd", ".hdr", ".pic",  ".pbm", ".pgm", ".pnm", ".ppm"};
constexpr inline std::array<std::string_view, 4> sound{".ogg", ".flac", ".mp3", ".wav"};
} // namespace formats

constexpr inline std::string_view game_textures{"$game_textures$"};

constexpr inline std::string_view level{"$level$"};
constexpr inline std::string_view game_saves{"$game_saves$"};
} // namespace fsgame
} // namespace xr

enum FS_List : u32
{
    FS_ListFiles = (1 << 0),
    FS_ListFolders = (1 << 1),
    FS_ClampExt = (1 << 2),
    FS_RootOnly = (1 << 3),
    FS_NoLower = 1 << 4,
};

class FS_Path
{
public:
    enum
    {
        flRecurse = (1 << 0),
        flNotif = (1 << 1),
        flNeedRescan = (1 << 2),
    };

public:
    LPSTR m_Path;
    LPSTR m_Root;
    LPSTR m_Add;
    LPSTR m_DefExt;
    LPSTR m_FilterCaption;
    Flags32 m_Flags;

public:
    explicit FS_Path(LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt = nullptr, LPCSTR _FilterString = nullptr, u32 flags = 0);
    ~FS_Path();

    [[nodiscard]] LPCSTR _update(string_path& dest, LPCSTR src) const;

    void _set(LPSTR add);
    void _set_root(LPSTR root);
};

struct __finddata64_t;
using _FINDDATA_T = __finddata64_t;

struct FS_File
{
    enum
    {
        flSubDir = (1 << 0),
        flVFS = (1 << 1),
    };

    u32 attrib{};
    s64 time_write{};
    s64 size{};
    xr_string name; // low-case name

    void set(const xr_string& nm, s64 sz, s64 modif, u32 attr, bool lower = true);

public:
    FS_File() = default;
    explicit FS_File(const xr_string& nm);
    explicit FS_File(const _FINDDATA_T& f);
    explicit FS_File(const xr_string& nm, const _FINDDATA_T& f);
    explicit FS_File(const xr_string& nm, s64 sz, s64 modif, u32 attr);
    explicit FS_File(const xr_string& nm, s64 sz, s64 modif, u32 attr, bool lower);

    [[nodiscard]] constexpr auto operator<=>(const FS_File& _X) const { return xr_strcmp(name, _X.name); }
};
DEFINE_SET(FS_File, FS_FileSet, FS_FileSetIt);

[[nodiscard]] bool PatternMatch(LPCSTR s, LPCSTR mask);
