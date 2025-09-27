#include "stdafx.h"

#include "fs_registrator.h"
#include "LocatorApi.h"

static LPCSTR update_path_script(CLocatorAPI* fs, LPCSTR initial, LPCSTR src)
{
    string_path temp;
    shared_str temp_2;
    fs->update_path(temp, initial, src);
    temp_2 = temp;
    return *temp_2;
}

class FS_file_list
{
    xr_vector<LPSTR>* m_p;

public:
    FS_file_list(xr_vector<LPSTR>* p) : m_p{p} {}

    u32 Size() { return m_p->size(); }
    LPCSTR GetAt(u32 idx) { return m_p->at(idx); }
    void Free() { FS.file_list_close(m_p); }
};

struct FS_item
{
    string_path name;
    u32 size;
    u32 modif;
    string256 buff;

    LPCSTR NameShort() { return name; }
    LPCSTR NameFull() { return name; }
    u32 Size() { return size; }
    LPCSTR Modif()
    {
        struct tm* newtime;
        time_t t = modif;
        newtime = localtime(&t);
        strcpy_s(buff, asctime(newtime));
        return buff;
    }

    LPCSTR ModifDigitOnly()
    {
        struct tm* newtime;
        time_t t = modif;
        newtime = localtime(&t);
        sprintf_s(buff, "%02d:%02d:%4d %02d:%02d", newtime->tm_mday, newtime->tm_mon + 1, newtime->tm_year + 1900, newtime->tm_hour, newtime->tm_min);
        return buff;
    }
};

template <bool b>
static bool sizeSorter(const FS_item& itm1, const FS_item& itm2)
{
    if (b)
        return (itm1.size < itm2.size);
    return (itm2.size < itm1.size);
}

template <bool b>
static bool modifSorter(const FS_item& itm1, const FS_item& itm2)
{
    if (b)
        return (itm1.modif < itm2.modif);
    return (itm2.modif < itm1.modif);
}

template <bool b>
static bool nameSorter(const FS_item& itm1, const FS_item& itm2)
{
    if (b)
        return (xr_strcmp(itm1.name, itm2.name) < 0);
    return (xr_strcmp(itm2.name, itm1.name) < 0);
}

class FS_file_list_ex
{
    xr_vector<FS_item> m_file_items;

public:
    enum
    {
        eSortByNameUp = 0,
        eSortByNameDown,
        eSortBySizeUp,
        eSortBySizeDown,
        eSortByModifUp,
        eSortByModifDown
    };
    FS_file_list_ex(LPCSTR path, u32 flags, LPCSTR mask);

    u32 Size() { return m_file_items.size(); }
    FS_item GetAt(u32 idx) { return m_file_items[idx]; }
    void Sort(u32 flags);
    xr_vector<FS_item>& GetAll() { return m_file_items; }
};

FS_file_list_ex::FS_file_list_ex(LPCSTR path, u32 flags, LPCSTR mask)
{
    FS_Path* P = FS.get_path(path);
    P->m_Flags.set(FS_Path::flNeedRescan, TRUE);
    FS.m_Flags.set(CLocatorAPI::flNeedCheck, TRUE);
    FS.rescan_physical_pathes();

    FS_FileSet files;
    FS.file_list(files, path, flags, mask);

    for (FS_FileSetIt it = files.begin(); it != files.end(); ++it)
    {
        auto& itm = m_file_items.emplace_back();
        ZeroMemory(itm.name, sizeof(itm.name));
        strcat_s(itm.name, it->name.c_str());
        itm.modif = (u32)it->time_write;
        itm.size = it->size;
    }

    FS.m_Flags.set(CLocatorAPI::flNeedCheck, FALSE);
}

void FS_file_list_ex::Sort(u32 flags)
{
    if (flags == eSortByNameUp)
        std::sort(m_file_items.begin(), m_file_items.end(), nameSorter<true>);
    else if (flags == eSortByNameDown)
        std::sort(m_file_items.begin(), m_file_items.end(), nameSorter<false>);
    else if (flags == eSortBySizeUp)
        std::sort(m_file_items.begin(), m_file_items.end(), sizeSorter<true>);
    else if (flags == eSortBySizeDown)
        std::sort(m_file_items.begin(), m_file_items.end(), sizeSorter<false>);
    else if (flags == eSortByModifUp)
        std::sort(m_file_items.begin(), m_file_items.end(), modifSorter<true>);
    else if (flags == eSortByModifDown)
        std::sort(m_file_items.begin(), m_file_items.end(), modifSorter<false>);
}

static FS_file_list_ex file_list_open_ex(CLocatorAPI* fs, LPCSTR path, u32 flags, LPCSTR mask) { return FS_file_list_ex(path, flags, mask); }

static FS_file_list file_list_open_script(CLocatorAPI* fs, LPCSTR initial, u32 flags) { return FS_file_list(fs->file_list_open(initial, flags)); }

static FS_file_list file_list_open_script_2(CLocatorAPI* fs, LPCSTR initial, LPCSTR folder, u32 flags) { return FS_file_list(fs->file_list_open(initial, folder, flags)); }

static LPCSTR get_file_age_str(CLocatorAPI* fs, LPCSTR nm)
{
    time_t t = fs->get_file_age(nm);
    struct tm* newtime;
    newtime = localtime(&t);
    return asctime(newtime);
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// SCRIPT C++17 FILESYSTEM - START ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#include <iomanip>
#include <sstream>
#include <filesystem>
namespace stdfs = std::filesystem;

// Путь до папки с движком
static std::string get_engine_dir() { return stdfs::current_path().string(); }

// Перебор файлов в папке, подкаталоги не учитываются.
static void directory_iterator(const char* dir, sol::function iterator_func)
{
    if (!stdfs::exists(dir))
        return;

    for (const auto& file : stdfs::directory_iterator(dir))
        if (stdfs::is_regular_file(file)) // Папки не учитываем
            iterator_func(file);
}

// Перебор файлов в папке включая подкаталоги.
static void recursive_directory_iterator(const char* dir, sol::function iterator_func)
{
    if (!stdfs::exists(dir))
        return;

    for (const auto& file : stdfs::recursive_directory_iterator(dir))
        if (stdfs::is_regular_file(file)) // Папки не учитываем
            iterator_func(file);
}

// полный путь до файла с расширением.
static std::string get_full_path(const stdfs::directory_entry& file) { return file.path().string(); }

// имя файла без пути, но с расширением.
static std::string get_full_filename(const stdfs::directory_entry& file) { return file.path().filename().string(); }

// имя файла без пути, и без расширения.
static std::string get_short_filename(const stdfs::directory_entry& file) { return file.path().stem().string(); }

// расширение файла.
static std::string get_extension(const stdfs::directory_entry& file) { return file.path().extension().string(); }

// Время последнего изменения файла
static decltype(auto) get_last_write_time(const stdfs::directory_entry& file)
{
    const auto sys = std::chrono::file_clock::to_sys(file.last_write_time());
    return decltype(sys)::clock::to_time_t(std::chrono::time_point_cast<std::chrono::microseconds>(sys));
}

static auto format_last_write_time = [](const stdfs::directory_entry& file, const char* fmt) {
    static std::ostringstream ss;
    static const std::locale loc{""};
    if (loc != ss.getloc())
        ss.imbue(loc); // Устанавливаем системную локаль потоку, чтоб месяц/день недели были на системном языке.

    ss.str("");

    const auto write_time_c = get_last_write_time(file);

    ss << std::put_time(std::localtime(&write_time_c), fmt);
    return ss.str();
};

// Время последнего изменения файла в формате [вторник 02 янв 2018 14:03:32]
static std::string get_last_write_time_string(const stdfs::directory_entry& file) { return format_last_write_time(file, "[%A %d %b %Y %T]"); }

// Время последнего изменения файла в формате [02:01:2018 14:03:32]
static std::string get_last_write_time_string_short(const stdfs::directory_entry& file) { return format_last_write_time(file, "[%d:%m:%Y %T]"); }

static void script_register_stdfs(sol::state_view& lua)
{
    auto fs = lua.create_named_table("stdfs", "VerifyPath", &VerifyPath, "directory_iterator", &directory_iterator, "recursive_directory_iterator", &recursive_directory_iterator);

    using self = stdfs::directory_entry;
    fs.new_usertype<self>("path", sol::no_constructor, sol::call_constructor, sol::constructors<self(const char*)>(),
                          // TODO: при необходимости можно будет добавить возможность изменения некоторых свойств.
                          "full_path_name", sol::property(&get_full_path), "full_filename", sol::property(&get_full_filename), "short_filename", sol::property(&get_short_filename),
                          "extension", sol::property(&get_extension), "last_write_time", sol::property(&get_last_write_time), "last_write_time_string",
                          sol::property(&get_last_write_time_string), "last_write_time_string_short", sol::property(&get_last_write_time_string_short), "exists",
                          sol::resolve<bool() const>(&self::exists), "is_regular_file", sol::resolve<bool() const>(&self::is_regular_file), "is_directory",
                          sol::resolve<bool() const>(&self::is_directory), "file_size", sol::resolve<uintmax_t() const>(&self::file_size));
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// SCRIPT C++17 FILESYSTEM - END ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void fs_registrator::script_register(sol::state_view& lua)
{
    //
    script_register_stdfs(lua);
    //

    lua.new_usertype<FS_item>("FS_item", sol::no_constructor, "NameFull", &FS_item::NameFull, "NameShort", &FS_item::NameShort, "Size", &FS_item::Size, "ModifDigitOnly",
                              &FS_item::ModifDigitOnly, "Modif", &FS_item::Modif);

    lua.new_usertype<FS_file_list_ex>("FS_file_list_ex", sol::no_constructor, "Size", &FS_file_list_ex::Size, "GetAt", &FS_file_list_ex::GetAt, "Sort", &FS_file_list_ex::Sort,
                                      "GetAll", &FS_file_list_ex::GetAll);
    lua.new_usertype<FS_file_list>("FS_file_list", sol::no_constructor, "Size", &FS_file_list::Size, "GetAt", &FS_file_list::GetAt, "Free", &FS_file_list::Free);

    lua.new_usertype<CLocatorAPI::file>("fs_file", sol::no_constructor, "name", sol::readonly(&CLocatorAPI::file::name), "vfs", sol::readonly(&CLocatorAPI::file::vfs), "ptr",
                                        sol::readonly(&CLocatorAPI::file::ptr), "size_real", sol::readonly(&CLocatorAPI::file::size_real), "size_compressed",
                                        sol::readonly(&CLocatorAPI::file::size_compressed), "modif", sol::readonly(&CLocatorAPI::file::modif));

    lua.new_usertype<CLocatorAPI>(
        "FS", sol::no_constructor,

        // FS_sort_mode
        "FS_sort_by_name_up", sol::var(FS_file_list_ex::eSortByNameUp), "FS_sort_by_name_down", sol::var(FS_file_list_ex::eSortByNameDown), "FS_sort_by_size_up",
        sol::var(FS_file_list_ex::eSortBySizeUp), "FS_sort_by_size_down", sol::var(FS_file_list_ex::eSortBySizeDown), "FS_sort_by_modif_up",
        sol::var(FS_file_list_ex::eSortByModifUp), "FS_sort_by_modif_down", sol::var(FS_file_list_ex::eSortByModifDown),
        // FS_List
        "FS_ListFiles", sol::var(FS_ListFiles), "FS_ListFolders", sol::var(FS_ListFolders), "FS_ClampExt", sol::var(FS_ClampExt), "FS_RootOnly", sol::var(FS_RootOnly),
        "FS_NoLower", sol::var(FS_NoLower),

        "path_exist", &CLocatorAPI::path_exist, "update_path", &update_path_script, "get_path", &CLocatorAPI::get_path, "append_path", &CLocatorAPI::append_path,

        "file_delete", sol::overload(sol::resolve<void(LPCSTR, LPCSTR)>(&CLocatorAPI::file_delete), sol::resolve<void(LPCSTR)>(&CLocatorAPI::file_delete)), "application_dir",
        &get_engine_dir, "file_rename", &CLocatorAPI::file_rename, "file_length", &CLocatorAPI::file_length, "file_copy", &CLocatorAPI::file_copy,

        "exist", sol::overload(sol::resolve<const CLocatorAPI::file*(LPCSTR)>(&CLocatorAPI::exist), sol::resolve<const CLocatorAPI::file*(LPCSTR, LPCSTR)>(&CLocatorAPI::exist)),
        "get_file_age", &CLocatorAPI::get_file_age, "get_file_age_str", &get_file_age_str,

        "r_open", sol::overload(sol::resolve<IReader*(LPCSTR, LPCSTR)>(&CLocatorAPI::r_open), sol::resolve<IReader*(LPCSTR)>(&CLocatorAPI::r_open)), "r_close",
        [](CLocatorAPI& self, IReader* file) { self.r_close(file); }, "w_open", sol::resolve<IWriter*(LPCSTR, LPCSTR)>(&CLocatorAPI::w_open), "w_close",
        [](CLocatorAPI& self, IWriter* file) { self.w_close(file); },

        "file_list_open", sol::overload(&file_list_open_script, &file_list_open_script_2), "file_list_open_ex", &file_list_open_ex);

    lua.set_function("getFS", [] { return &FS; });
}
