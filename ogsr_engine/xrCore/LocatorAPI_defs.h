#pragma once

namespace xr
{
namespace fsgame
{
namespace formats
{
constexpr inline std::array<std::string_view, 12> texture{".exr", ".ktx2", ".ktx", ".dds", ".qoi", ".png", ".tga", ".jpg", ".jpeg", ".bmp", ".hdr", ".psd"};
constexpr inline std::array<std::string_view, 6> font{".otf", ".ttf", ".woff2", ".woff", ".otc", ".ttc"};
constexpr inline std::array<std::string_view, 6> sound{".flac", ".opus", ".ogg", ".oga", ".mp3", ".wav"};
constexpr inline std::array<std::string_view, 8> archive{".dw", ".sq", ".z", ".xdb", ".db", ".7z", ".r", ".tar"};
} // namespace formats

constexpr inline std::string_view app_data_root{"$app_data_root$"};

constexpr inline std::string_view game_sounds{"$game_sounds$"};
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
    void _set_root(gsl::czstring root);
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
