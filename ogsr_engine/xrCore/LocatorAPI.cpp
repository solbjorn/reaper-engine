// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "FS_internal.h"
#include "stream_reader.h"
#include "file_stream_reader.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <sqfs/super.h>

XR_DIAG_POP();

#include <filesystem>

#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>

std::unique_ptr<CLocatorAPI> xr_FS;

struct _open_file
{
    union
    {
        IReader* _reader;
        CStreamReader* _stream_reader;
    };
    shared_str _fn;
    size_t _used;
};

template <typename T>
struct eq_pointer;

template <>
struct eq_pointer<IReader>
{
    IReader* _val;
    eq_pointer(IReader* p) : _val(p) {}
    bool operator()(_open_file& itm) const { return _val == itm._reader; }
};

template <>
struct eq_pointer<CStreamReader>
{
    CStreamReader* _val;
    eq_pointer(CStreamReader* p) : _val(p) {}
    bool operator()(_open_file& itm) const { return _val == itm._stream_reader; }
};

struct eq_fname_free
{
    shared_str _val;
    eq_fname_free(shared_str s) : _val(s) {}
    bool operator()(_open_file& itm) const { return _val == itm._fn && itm._reader == nullptr; }
};

struct eq_fname_check
{
    shared_str _val;
    eq_fname_check(shared_str s) : _val(s) {}
    bool operator()(_open_file& itm) const { return _val == itm._fn && itm._reader != nullptr; }
};

xr_vector<_open_file> g_open_files;

void _check_open_file(const shared_str& _fname)
{
    auto it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_fname_check(_fname));
    if (it != g_open_files.end())
        Msg("file opened at least twice: [%s]", _fname.c_str());
}

_open_file& find_free_item(const shared_str& _fname)
{
    auto it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_fname_free(_fname));
    if (it == g_open_files.end())
    {
        g_open_files.resize(g_open_files.size() + 1);
        _open_file& _of = g_open_files.back();
        _of._fn = _fname;
        _of._used = 0;
        return _of;
    }
    return *it;
}

void setup_reader(CStreamReader* _r, _open_file& _of) { _of._stream_reader = _r; }

void setup_reader(IReader* _r, _open_file& _of) { _of._reader = _r; }

template <typename T>
void _register_open_file(T* _r, LPCSTR _fname)
{
    xrCriticalSection _lock;
    _lock.Enter();

    shared_str f = _fname;
    _check_open_file(f);

    _open_file& _of = find_free_item(_fname);
    setup_reader(_r, _of);
    _of._used += 1;

    _lock.Leave();
}

template <typename T>
void _unregister_open_file(T* _r)
{
    xrCriticalSection _lock;
    _lock.Enter();

    auto it = std::find_if(g_open_files.begin(), g_open_files.end(), eq_pointer<T>(_r));
    VERIFY(it != g_open_files.end());
    _open_file& _of = *it;
    _of._reader = nullptr;
    _lock.Leave();
}

void _dump_open_files(int mode)
{
    bool bShow = false;
    if (mode == 1)
    {
        for (const auto& file : g_open_files)
        {
            if (file._reader != nullptr)
            {
                if (!bShow)
                    Log("----opened files");

                bShow = true;
                Msg("[%zu] fname:%s", file._used, file._fn.c_str());
            }
        }
    }
    else
    {
        Log("----un-used");
        for (const auto& file : g_open_files)
        {
            if (file._reader == nullptr)
                Msg("[%zu] fname:%s", file._used, file._fn.c_str());
        }
    }
    if (bShow)
        Msg("----total count = [%zu]", g_open_files.size());
}

CLocatorAPI::CLocatorAPI()
#ifdef PROFILE_CRITICAL_SECTIONS
    : m_auth_lock(MUTEX_PROFILE_ID(CLocatorAPI::m_auth_lock))
#endif // PROFILE_CRITICAL_SECTIONS
{
    m_Flags.zero();
    // get page size
    SYSTEM_INFO sys_inf;
    GetSystemInfo(&sys_inf);
    dwAllocGranularity = sys_inf.dwAllocationGranularity;
    m_iLockRescan = 0;
    dwOpenCounter = 0;
}

CLocatorAPI::~CLocatorAPI()
{
    VERIFY(0 == m_iLockRescan);
    _dump_open_files(1);
}

void CLocatorAPI::Register(LPCSTR name, size_t vfs, u32 ptr, u32 size_real, u32 size_compressed, u32 modif)
{
    string256 temp_file_name;
    strcpy_s(temp_file_name, name);
    xr_strlwr(temp_file_name);

    // Register file
    file desc;
    //	desc.name			= xr_strlwr(xr_strdup(name));
    desc.name = temp_file_name;
    desc.vfs = vfs;
    desc.ptr = ptr;
    desc.size_real = size_real;
    desc.size_compressed = size_compressed;
    desc.modif = modif & ~u32(0x3);

    //	if file already exist - update info
    files_it I = files.find(desc);
    if (I != files.end())
    {
        desc.name = I->name;
        // sad but true, performance option
        // correct way is to erase and then insert new record:
        const_cast<file&>(*I) = desc;

        return;
    }
    else
    {
        desc.name = xr_strdup(desc.name);
    }

    // otherwise insert file
    files.insert(desc);

    // Try to register folder(s)
    string_path temp;
    strcpy_s(temp, desc.name);
    string_path path;
    string_path folder;
    while (temp[0])
    {
        _splitpath(temp, path, folder, nullptr, nullptr);
        if (!folder[0])
            break;

        strcat_s(path, folder);
        if (!exist(path))
        {
            desc.name = xr_strdup(path);
            desc.vfs = VFS_STANDARD_FILE;
            desc.ptr = 0;
            desc.size_real = 0;
            desc.size_compressed = 0;
            desc.modif = u32(-1);
            desc.folder = true;

            auto [it, done] = files.insert(desc);
            R_ASSERT(done);
        }

        strcpy_s(temp, path); // strcpy_s(temp, folder);
        if (xr_strlen(temp))
            temp[xr_strlen(temp) - 1] = 0;
    }
}

/* Archives */

void CLocatorAPI::archive::open()
{
    // Open the file
    if (hSrcFile && hSrcMap)
        return;

    hSrcFile = CreateFile(*path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    R_ASSERT(hSrcFile != INVALID_HANDLE_VALUE);

    LARGE_INTEGER sz;
    GetFileSizeEx(hSrcFile, &sz);
    size = sz.QuadPart;
    R_ASSERT(size > 0);

    u32 dwPtr = SetFilePointer(hSrcFile, 0, nullptr, FILE_BEGIN);
    R_ASSERT3(dwPtr != INVALID_SET_FILE_POINTER, *path, Debug.error2string(GetLastError()));

    u32 magic = u32(-1);
    DWORD read_byte;
    bool res = ReadFile(hSrcFile, &magic, 4, &read_byte, nullptr);
    R_ASSERT3(res && read_byte == sizeof(u32), *path, Debug.error2string(GetLastError()));

    if (magic == SQFS_MAGIC)
        open_sqfs();
    else
        open_db();
}

IC bool CLocatorAPI::archive::autoload() { return type == container::SQFS ? autoload_sqfs() : autoload_db(); }
IC const char* CLocatorAPI::archive::entry_point() { return type == container::SQFS ? entry_point_sqfs() : entry_point_db(); }

IC void CLocatorAPI::archive::index(CLocatorAPI& loc, const char* fs_entry_point)
{
    if (type == container::SQFS)
        index_sqfs(loc, fs_entry_point);
    else
        index_db(loc, fs_entry_point);
}

IC IReader* CLocatorAPI::archive::read(const char* fname, const struct file& desc, u32 gran)
{
    return type == container::SQFS ? read_sqfs(fname, desc, gran) : read_db(fname, desc, gran);
}

IC CStreamReader* CLocatorAPI::archive::stream(const char* fname, const struct file& desc) { return type == container::SQFS ? stream_sqfs(fname, desc) : stream_db(fname, desc); }

IC void CLocatorAPI::archive::cleanup()
{
    if (type == container::SQFS)
        cleanup_sqfs();
    else
        cleanup_db();
}

void CLocatorAPI::archive::close()
{
    if (type == container::SQFS)
        close_sqfs();
    else
        close_db();

    CloseHandle(hSrcFile);
    hSrcFile = nullptr;
}

void CLocatorAPI::LoadArchive(archive& A)
{
    // Create base path
    string_path fs_entry_point;
    fs_entry_point[0] = 0;

    shared_str read_path = A.entry_point() ?: "gamedata";
    if (!_stricmp(read_path.c_str(), "gamedata"))
    {
        read_path = "$fs_root$";
        PathPairIt P = pathes.find(read_path.c_str());
        if (P != pathes.end())
        {
            FS_Path* root = P->second;
            xr_strcpy(fs_entry_point, sizeof(fs_entry_point), root->m_Path);
        }
        xr_strcat(fs_entry_point, "gamedata\\");
    }
    else
    {
        string256 alias_name;
        alias_name[0] = 0;
        R_ASSERT2(*read_path.c_str() == '$', read_path.c_str());

        int count = sscanf(read_path.c_str(), "%[^\\]s", alias_name);
        R_ASSERT2(count == 1, read_path.c_str());

        PathPairIt P = pathes.find(alias_name);

        if (P != pathes.end())
        {
            FS_Path* root = P->second;
            xr_strcpy(fs_entry_point, sizeof(fs_entry_point), root->m_Path);
        }
        xr_strcat(fs_entry_point, sizeof(fs_entry_point), read_path.c_str() + xr_strlen(alias_name) + 1);
    }

    // Read FileSystem
    A.open();
    A.index(*this, fs_entry_point);
}

void CLocatorAPI::ProcessArchive(LPCSTR _path)
{
    // find existing archive
    shared_str path = _path;

    for (archives_it it = archives.begin(); it != archives.end(); ++it)
        if (it->path == path)
            return;

    // open archive
    auto& A = archives.emplace_back();
    A.vfs_idx = archives.size() - 1;
    A.path = path;

    A.open();

    bool bProcessArchiveLoading = A.autoload();
    if (bProcessArchiveLoading || strstr(Core.Params, "-auto_load_arch"))
        LoadArchive(A);
    else
        A.close();
}

void CLocatorAPI::ProcessOne(LPCSTR path, const _finddata_t& F, bool bNoRecurse)
{
    string_path N;
    strcpy_s(N, path);
    strcat_s(N, F.name);
    xr_strlwr(N);

    if (F.attrib & _A_HIDDEN)
    {
        return;
    }

    if (F.attrib & _A_SUBDIR)
    {
        if (bNoRecurse)
            return;

        if (0 == xr_strcmp(F.name, "."))
            return;
        if (0 == xr_strcmp(F.name, ".."))
            return;

        strcat_s(N, "\\");

        RecurseScanPhysicalPath(N, false, bNoRecurse);
    }
    else
    {
        if (!m_Flags.is(flTargetFolderOnly) && strext(N) && (!strncmp(strext(N), ".db", 3) || !strncmp(strext(N), ".xdb", 4) || !strncmp(strext(N), ".sq", 3)))
        {
            Msg("--Found base arch: [%s], size: [%lu]", N, F.size);
            ProcessArchive(N);
        }
        else
        {
            Register(N, VFS_STANDARD_FILE, 0, F.size, F.size, (u32)F.time_write);
        }
    }
}

bool ignore_name(const char* _name)
{
    // ignore windows hidden Thumbs.db
    if (!strcmp(_name, "Thumbs.db"))
        return true;

    // ignore processing ".svn" folders
    if (!strcmp(_name, ".svn"))
        return true;

    // ignore processing ".git" folders
    if (!strcmp(_name, ".git"))
        return true;

    return false;
}

// we need to check for file existance
// because Unicode file names can
// be interpolated by FindNextFile()

bool ignore_path(const char* _path) { return !std::filesystem::exists(_path); }

bool CLocatorAPI::RecurseScanPhysicalPath(const char* path, const bool log_if_found, bool bNoRecurse)
{
    _finddata_t sFile;
    intptr_t hFile;

    string_path N;
    strcpy_s(N, path);
    strcat_s(N, "*.*");

    using FFVec = xr_vector<_finddata_t>;
    FFVec rec_files;
    rec_files.reserve(1224);

    // find all files
    if (-1 == (hFile = _findfirst(N, &sFile)))
    {
        // Log		("! Wrong path: ",path);
        return false;
    }

    if (log_if_found)
        Msg("--Found FS dir: [%s]", path);

    string1024 full_path;
    if (m_Flags.test(flNeedCheck))
    {
        strcpy_s(full_path, path);
        strcat_s(full_path, sFile.name);

        // загоняем в вектор для того *.db* приходили в сортированном порядке
        if (!ignore_name(sFile.name) && !ignore_path(full_path))
            rec_files.push_back(sFile);

        while (_findnext(hFile, &sFile) == 0)
        {
            strcpy_s(full_path, path);
            strcat_s(full_path, sFile.name);
            if (!ignore_name(sFile.name) && !ignore_path(full_path))
                rec_files.push_back(sFile);
        }
    }
    else
    {
        // загоняем в вектор для того *.db* приходили в сортированном порядке
        if (!ignore_name(sFile.name))
            rec_files.push_back(sFile);

        while (_findnext(hFile, &sFile) == 0)
        {
            if (!ignore_name(sFile.name))
                rec_files.push_back(sFile);
        }
    }

    _findclose(hFile);

    if (log_if_found)
        Msg("  files: [%zu]", rec_files.size());

    std::ranges::sort(rec_files, [](const _finddata_t& x, const _finddata_t& y) { return xr_strcmp(x.name, y.name) < 0; });

    for (const auto& el : rec_files)
    {
        ProcessOne(path, el, bNoRecurse);
    }

    // insert self
    if (path && path[0])
    {
        Register(path, VFS_STANDARD_FILE, 0, 0, 0, 0);
    }

    return true;
}

void CLocatorAPI::_initialize(u32 flags, LPCSTR target_folder, LPCSTR fs_name)
{
    char _delimiter = '|'; //','
    if (m_Flags.is(flReady))
        return;
    CTimer t;
    t.Start();
    Log("Initializing File System...");
    size_t M1 = Memory.mem_usage();

    m_Flags.set(flags, TRUE);

    // scan root directory
    string4096 buf;
    IReader* pFSltx{};

    // append working folder
    LPCSTR fs_ltx{};

    // append application path
    if (m_Flags.is(flScanAppRoot))
        append_path("$app_root$", Core.ApplicationPath, nullptr, FALSE);

    if (m_Flags.is(flTargetFolderOnly))
    {
        append_path("$fs_root$", "", nullptr, FALSE);
    }
    else
    {
        // find nearest fs.ltx and set fs_root correctly
        fs_ltx = (fs_name && fs_name[0]) ? fs_name : FSLTX;
        pFSltx = r_open(fs_ltx);

        if (!pFSltx && m_Flags.is(flScanAppRoot))
            pFSltx = r_open("$app_root$", fs_ltx);

        if (!pFSltx)
        {
            if (strstr(Core.Params, "-use-work-dir"))
            {
                string_path currentDir;
                GetCurrentDirectory(std::size(currentDir) - 1, currentDir);
                currentDir[std::size(currentDir) - 1] = '\0';
                append_path("$fs_root$", currentDir, nullptr, FALSE);
            }
            else
            {
                string_path tmpAppPath{};
                strcpy_s(tmpAppPath, Core.ApplicationPath);
                if (xr_strlen(tmpAppPath))
                {
                    tmpAppPath[xr_strlen(tmpAppPath) - 1] = 0;
                    if (strrchr(tmpAppPath, '\\'))
                        *(strrchr(tmpAppPath, '\\') + 1) = 0;

                    append_path("$fs_root$", tmpAppPath, nullptr, FALSE);
                }
                else
                    append_path("$fs_root$", "", nullptr, FALSE);
            }

            pFSltx = r_open("$fs_root$", fs_ltx);
        }
        else
            append_path("$fs_root$", "", nullptr, FALSE);

        Msg("using fs-ltx: [%s]", fs_ltx);
    }

    //-----------------------------------------------------------
    // append application data path
    // target folder
    if (m_Flags.is(flTargetFolderOnly))
    {
        append_path("$target_folder$", target_folder, nullptr, TRUE);
    }
    else
    {
        CHECK_OR_EXIT(pFSltx, make_string("Cannot open file \"%s\".\nCheck your working folder.", fs_ltx));
        // append all pathes
        string_path id, root, add, def, capt;
        LPCSTR lp_add, lp_def, lp_capt;
        string16 b_v;
        string4096 temp;

        while (!pFSltx->eof())
        {
            pFSltx->r_string(buf, sizeof(buf));
            if (buf[0] == ';')
                continue;

            _GetItem(buf, 0, id, '=');

            if (!m_Flags.is(flBuildCopy) && (0 == xr_strcmp(id, "$build_copy$")))
                continue;

            _GetItem(buf, 1, temp, '=');
            int cnt = _GetItemCount(temp, _delimiter);
            R_ASSERT2(cnt >= 3, temp);
            u32 fl = 0;
            _GetItem(temp, 0, b_v, _delimiter);

            if (CInifile::IsBOOL(b_v))
                fl |= FS_Path::flRecurse;

            _GetItem(temp, 1, b_v, _delimiter);
            if (CInifile::IsBOOL(b_v))
                fl |= FS_Path::flNotif;

            _GetItem(temp, 2, root, _delimiter);
            _GetItem(temp, 3, add, _delimiter);
            _GetItem(temp, 4, def, _delimiter);
            _GetItem(temp, 5, capt, _delimiter);
            xr_strlwr(id);

            xr_strlwr(root);
            lp_add = (cnt >= 4) ? xr_strlwr(add) : nullptr;
            lp_def = (cnt >= 5) ? def : nullptr;
            lp_capt = (cnt >= 6) ? capt : nullptr;

            PathPairIt p_it = pathes.find(root);

            std::pair<PathPairIt, bool> I;
            FS_Path* P = xr_new<FS_Path>((p_it != pathes.end()) ? p_it->second->m_Path : root, lp_add, lp_def, lp_capt, fl);
            I = pathes.emplace(xr_strdup(id), P);

#ifndef DEBUG
            m_Flags.set(flCacheFiles, FALSE);
#endif // DEBUG

            CHECK_OR_EXIT(I.second, "The file 'fsgame.ltx' is corrupted (it contains duplicated lines).\nPlease reinstall the game or fix the problem manually.");
        }
        r_close(pFSltx);
        R_ASSERT(path_exist("$app_data_root$"));

        for (PathPairIt p_it = pathes.begin(); p_it != pathes.end(); p_it++)
        {
            FS_Path* P = p_it->second;
#ifdef RESTRICT_GAMEDATA
            shared_str id = p_it->first;
            if (!strcmp(id, "$app_data_root$") || !strcmp(id, "$game_saves$") || !strcmp(id, "$logs$") || !strcmp(id, "$screenshots$"))
                RecurseScanPhysicalPath(P->m_Path);
#else
            RecurseScanPhysicalPath(P->m_Path, true, !P->m_Flags.test(FS_Path::flRecurse));
#endif
        }
    }

    const size_t M2 = Memory.mem_usage();
    Msg("FS: %zu files cached, %zu Kb memory used.", files.size(), (M2 - M1) / 1024);

    m_Flags.set(flReady, TRUE);

    Msg("Init FileSystem %f sec", t.GetElapsed_sec());
    //-----------------------------------------------------------
    if (strstr(Core.Params, "-overlaypath"))
    {
        string1024 c_newAppPathRoot;
        sscanf(strstr(Core.Params, "-overlaypath ") + 13, "%[^ ] ", c_newAppPathRoot);
        FS_Path* pLogsPath = FS.get_path("$logs$");
        FS_Path* pAppdataPath = FS.get_path("$app_data_root$");

        if (pLogsPath)
            pLogsPath->_set_root(c_newAppPathRoot);
        if (pAppdataPath)
        {
            pAppdataPath->_set_root(c_newAppPathRoot);
            rescan_physical_path(pAppdataPath->m_Path, pAppdataPath->m_Flags.is(FS_Path::flRecurse));
        }
    }
    //-----------------------------------------------------------

    CreateLog(!!strstr(Core.Params, "-nolog"));
}

void CLocatorAPI::_destroy()
{
    for (auto& file : files)
    {
        char* str = const_cast<char*>(file.name);
        xr_free(str);
    }

    files.clear();

    for (auto& path : pathes)
    {
        char* str = const_cast<char*>(path.first);
        xr_free(str);

        xr_delete(path.second);
    }

    pathes.clear();

    for (auto& arc : archives)
    {
        arc.cleanup();
        arc.close();
    }

    archives.clear();
}

const CLocatorAPI::file* CLocatorAPI::exist(const char* fn)
{
    files_it it = file_find_it(fn);
    return (it != files.end()) ? &(*it) : nullptr;
}

const CLocatorAPI::file* CLocatorAPI::exist(const char* path, const char* name)
{
    string_path temp;
    update_path(temp, path, name);
    return exist(temp);
}

const CLocatorAPI::file* CLocatorAPI::exist(string_path& fn, LPCSTR path, LPCSTR name)
{
    update_path(fn, path, name);
    return exist(fn);
}

const CLocatorAPI::file* CLocatorAPI::exist(string_path& fn, LPCSTR path, LPCSTR name, LPCSTR ext)
{
    string_path nm;
    strconcat(sizeof(nm), nm, name, ext);
    update_path(fn, path, nm);
    return exist(fn);
}

xr_vector<char*>* CLocatorAPI::file_list_open(const char* initial, const char* folder, u32 flags)
{
    string_path N;
    R_ASSERT(initial && initial[0]);
    update_path(N, initial, folder);
    return file_list_open(N, flags);
}

xr_vector<char*>* CLocatorAPI::file_list_open(const char* _path, u32 flags)
{
    R_ASSERT(_path);
    VERIFY(flags);
    // проверить нужно ли пересканировать пути
    check_pathes();

    string_path N;

    if (path_exist(_path))
        update_path(N, _path, "");
    else
        strcpy_s(N, _path);

    file desc;
    desc.name = N;
    files_it I = files.find(desc);
    if (I == files.end())
        return nullptr;

    xr_vector<char*>* dest = xr_new<xr_vector<char*>>();

    size_t base_len = xr_strlen(N);
    for (++I; I != files.end(); I++)
    {
        const file& entry = *I;
        if (0 != strncmp(entry.name, N, base_len))
            break; // end of list
        const char* end_symbol = entry.name + xr_strlen(entry.name) - 1;
        if ((*end_symbol) != '\\')
        {
            // file
            if ((flags & FS_ListFiles) == 0)
                continue;

            const char* entry_begin = entry.name + base_len;
            if ((flags & FS_RootOnly) && strchr(entry_begin, '\\'))
                continue; // folder in folder
            dest->push_back(xr_strdup(entry_begin));
            LPSTR fname = dest->back();
            if (flags & FS_ClampExt)
                if (strext(fname))
                    *strext(fname) = 0;
        }
        else
        {
            // folder
            if ((flags & FS_ListFolders) == 0)
                continue;
            const char* entry_begin = entry.name + base_len;

            if ((flags & FS_RootOnly) && (strchr(entry_begin, '\\') != end_symbol))
                continue; // folder in folder

            dest->push_back(xr_strdup(entry_begin));
        }
    }
    return dest;
}

void CLocatorAPI::file_list_close(xr_vector<char*>*& lst)
{
    if (lst)
    {
        for (xr_vector<char*>::iterator I = lst->begin(); I != lst->end(); I++)
            xr_free(*I);
        xr_delete(lst);
    }
}

size_t CLocatorAPI::file_list(FS_FileSet& dest, LPCSTR path, u32 flags, LPCSTR mask)
{
    R_ASSERT(path);
    VERIFY(flags);
    // проверить нужно ли пересканировать пути
    check_pathes();

    string_path N;
    if (path_exist(path))
        update_path(N, path, "");
    else
        strcpy_s(N, path);

    file desc;
    desc.name = N;
    files_it I = files.find(desc);
    if (I == files.end())
        return 0;

    SStringVec masks;
    _SequenceToList(masks, mask);
    BOOL b_mask = !masks.empty();

    size_t base_len = xr_strlen(N);
    for (++I; I != files.end(); I++)
    {
        const file& entry = *I;
        if (0 != strncmp(entry.name, N, base_len))
            break; // end of list

        LPCSTR end_symbol = entry.name + xr_strlen(entry.name) - 1;
        if ((*end_symbol) != '\\')
        {
            // file
            if ((flags & FS_ListFiles) == 0)
                continue;

            LPCSTR entry_begin = entry.name + base_len;
            if ((flags & FS_RootOnly) && strchr(entry_begin, '\\'))
                continue; // folder in folder

            // check extension
            if (b_mask)
            {
                bool bOK = false;
                for (SStringVecIt it = masks.begin(); it != masks.end(); it++)
                {
                    if (PatternMatch(entry_begin, it->c_str()))
                    {
                        bOK = true;
                        break;
                    }
                }
                if (!bOK)
                    continue;
            }

            xr_string fn = entry_begin;

            // insert file entry
            if (flags & FS_ClampExt)
            {
                if (LPCSTR src_ext = strext(entry_begin); src_ext != nullptr)
                {
                    size_t ext_pos = src_ext - entry_begin;
                    fn.replace(ext_pos, strlen(src_ext), "");
                }
            }

            u32 fl = (entry.vfs != VFS_STANDARD_FILE ? FS_File::flVFS : 0);
            dest.emplace(fn, entry.size_real, entry.modif, fl, !(flags & FS_NoLower));
        }
        else
        {
            // folder
            if ((flags & FS_ListFolders) == 0)
                continue;
            LPCSTR entry_begin = entry.name + base_len;

            if ((flags & FS_RootOnly) && (strchr(entry_begin, '\\') != end_symbol))
                continue; // folder in folder
            u32 fl = FS_File::flSubDir | (entry.vfs ? FS_File::flVFS : 0);
            dest.emplace(entry_begin, entry.size_real, entry.modif, fl, !(flags & FS_NoLower));
        }
    }

    return dest.size();
}

void CLocatorAPI::file_from_cache_impl(IReader*& R, LPSTR fname, const file&) { R = xr_new<CVirtualFileReader>(fname); }

void CLocatorAPI::file_from_cache_impl(CStreamReader*& R, LPSTR fname, const file&)
{
    CFileStreamReader* r = xr_new<CFileStreamReader>();
    r->construct(fname, BIG_FILE_READER_WINDOW_SIZE);
    R = r;
}

template <typename T>
void CLocatorAPI::file_from_cache(T*& R, LPSTR fname, const file& desc, LPCSTR&)
{
    file_from_cache_impl(R, fname, desc);
}

void CLocatorAPI::file_from_archive(IReader*& R, LPCSTR fname, const file& desc)
{
    // Archived one
    R = archives[desc.vfs].read(fname, desc, dwAllocGranularity);
}

void CLocatorAPI::file_from_archive(CStreamReader*& R, LPCSTR fname, const file& desc)
{
    CStreamReader* reader = archives[desc.vfs].stream(fname, desc);
    R = reader;
}

bool CLocatorAPI::check_for_file(LPCSTR path, LPCSTR _fname, string_path& fname, const file*& desc)
{
    // проверить нужно ли пересканировать пути
    check_pathes();

    // correct path
    strcpy_s(fname, _fname);
    xr_strlwr(fname);
    if (path && path[0])
        update_path(fname, path, fname);

    // Search entry
    file desc_f;
    desc_f.name = fname;

    files_it I = files.find(desc_f);
    if (I == files.end())
        return (false);

    ++dwOpenCounter;
    desc = &*I;
    return (true);
}

template <typename T>
T* CLocatorAPI::r_open_impl(LPCSTR path, LPCSTR _fname)
{
    T* R = nullptr;
    string_path fname;
    const file* desc = nullptr;
    LPCSTR source_name = &fname[0];

    if (!check_for_file(path, _fname, fname, desc))
        return nullptr;

    // OK, analyse
    if (VFS_STANDARD_FILE == desc->vfs)
        file_from_cache(R, fname, *desc, source_name);
    else
        file_from_archive(R, fname, *desc);

    if (m_Flags.test(flDumpFileActivity))
        _register_open_file(R, fname);

    return (R);
}

CStreamReader* CLocatorAPI::rs_open(LPCSTR path, LPCSTR _fname) { return (r_open_impl<CStreamReader>(path, _fname)); }

IReader* CLocatorAPI::r_open(LPCSTR path, LPCSTR _fname) { return (r_open_impl<IReader>(path, _fname)); }

void CLocatorAPI::r_close(IReader*& fs)
{
    if (m_Flags.test(flDumpFileActivity))
        _unregister_open_file(fs);

    xr_delete(fs);
}

void CLocatorAPI::r_close(CStreamReader*& fs)
{
    if (m_Flags.test(flDumpFileActivity))
        _unregister_open_file(fs);

    fs->close();
}

IWriter* CLocatorAPI::w_open(LPCSTR path, LPCSTR _fname)
{
    string_path fname;
    strcpy_s(fname, _fname);
    xr_strlwr(fname); //,".$");
    if (path && path[0])
        update_path(fname, path, fname);
    CFileWriter* W = xr_new<CFileWriter>(fname, false);
    return W;
}

IWriter* CLocatorAPI::w_open_ex(LPCSTR path, LPCSTR _fname)
{
    string_path fname;
    strcpy_s(fname, _fname);
    xr_strlwr(fname); //,".$");
    if (path && path[0])
        update_path(fname, path, fname);
    CFileWriter* W = xr_new<CFileWriter>(fname, true);
    return W;
}

void CLocatorAPI::w_close(IWriter*& S)
{
    if (S)
    {
        R_ASSERT(S->fName.size());
        string_path fname;
        strcpy_s(fname, *S->fName);
        bool bReg = S->valid();
        xr_delete(S);

        if (bReg)
        {
            struct _stat st;
            _stat(fname, &st);
            Register(fname, VFS_STANDARD_FILE, 0, st.st_size, st.st_size, (u32)st.st_mtime);
        }
    }
}

CLocatorAPI::files_it CLocatorAPI::file_find_it(LPCSTR fname)
{
    // проверить нужно ли пересканировать пути
    check_pathes();

    file desc_f;
    desc_f.name = fname;

    return files.find(desc_f);
}

void CLocatorAPI::file_delete(LPCSTR path, LPCSTR nm)
{
    string_path fname;
    if (path && path[0])
        update_path(fname, path, nm);
    else
        strcpy_s(fname, nm);

    const files_it I = file_find_it(fname);
    if (I != files.end())
    {
        // remove file
        _unlink(I->name);

        char* str = const_cast<char*>(I->name);
        xr_free(str);

        files.erase(I);
    }
}

void CLocatorAPI::file_copy(LPCSTR src, LPCSTR dest)
{
#if __has_include("..\build_config_overrides\trivial_encryptor_ovr.h")
    FATAL("Mamkin Hacker Detected");
#else
    if (exist(src))
    {
        IReader* S = r_open(src);
        if (S)
        {
            IWriter* D = w_open(dest);
            if (D)
            {
                D->w(S->pointer(), S->length());
                w_close(D);
            }
            r_close(S);
        }
    }
#endif
}

void CLocatorAPI::file_rename(LPCSTR src, LPCSTR dest, bool bOwerwrite)
{
    files_it S = file_find_it(src);
    if (S != files.end())
    {
        files_it D = file_find_it(dest);
        if (D != files.end())
        {
            if (!bOwerwrite)
                return;

            _unlink(D->name);

            char* str = const_cast<char*>(D->name);
            xr_free(str);

            files.erase(D);

            S = file_find_it(src); // Обновим снова, потому что после erase итератор может быть невалидным
            R_ASSERT(S != files.end()); // на всякий случай
        }

        file new_desc = *S;

        // remove existing item
        char* str = const_cast<char*>(S->name);
        xr_free(str);

        files.erase(S);

        // insert updated item
        new_desc.name = xr_strlwr(xr_strdup(dest));
        files.insert(new_desc);

        // physically rename file
        VerifyPath(dest);
        rename(src, dest);
    }
}

int CLocatorAPI::file_length(LPCSTR src)
{
    auto I = file_find_it(src);
    return I != files.end() ? I->size_real : -1;
}

bool CLocatorAPI::path_exist(LPCSTR path)
{
    PathPairIt P = pathes.find(path);
    return (P != pathes.end());
}

FS_Path* CLocatorAPI::append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive)
{
    VERIFY(root);
    VERIFY(false == path_exist(path_alias));

    FS_Path* P = xr_new<FS_Path>(root, add, nullptr, nullptr, 0u);
    bool bNoRecurse = !recursive;

    RecurseScanPhysicalPath(P->m_Path, false, bNoRecurse);
    pathes.emplace(xr_strdup(path_alias), P);

    return P;
}

FS_Path* CLocatorAPI::get_path(LPCSTR path)
{
    PathPairIt P = pathes.find(path);
    R_ASSERT2(P != pathes.end(), path);
    return P->second;
}

LPCSTR CLocatorAPI::update_path(string_path& dest, LPCSTR initial, LPCSTR src) { return get_path(initial)->_update(dest, src); }

u32 CLocatorAPI::get_file_age(LPCSTR nm)
{
    // проверить нужно ли пересканировать пути
    check_pathes();

    files_it I = file_find_it(nm);
    return (I != files.end()) ? I->modif : u32(-1);
}

void CLocatorAPI::rescan_physical_path(LPCSTR full_path, BOOL bRecurse)
{
    auto I = file_find_it(full_path);
    if (I == files.end())
        return;

#ifdef DEBUG
    Msg("[rescan_physical_path] files count before: [%d]", files.size());
#endif

    const size_t base_len = strlen(full_path);

    while (I != files.end())
    {
        const file& entry = *I;

        if (0 != strncmp(entry.name, full_path, base_len))
            break; // end of list

        const char* entry_begin = entry.name + base_len;

        if (entry.vfs != VFS_STANDARD_FILE || entry.folder || (!bRecurse && strchr(entry_begin, '\\')))
        {
            I++;
        }
        else
        {
            // Msg("[rescan_physical_path] erace file: [%s]", entry.name);
            //  erase item
            char* str = const_cast<char*>(entry.name);
            xr_free(str);

            I = files.erase(I);
        }
    }

#ifdef DEBUG
    Msg("[rescan_physical_path] files count before2: [%u]", files.size());
#endif

    bool bNoRecurse = !bRecurse;
    RecurseScanPhysicalPath(full_path, false, bNoRecurse);

#ifdef DEBUG
    Msg("[rescan_physical_path] files count after: [%d]", files.size());
#endif
}

void CLocatorAPI::rescan_physical_pathes()
{
    m_Flags.set(flNeedRescan, FALSE);
    for (PathPairIt p_it = pathes.begin(); p_it != pathes.end(); p_it++)
    {
        FS_Path* P = p_it->second;
        if (P->m_Flags.is(FS_Path::flNeedRescan))
        {
            std::string filepath{P->m_Path};
            bool filepathExists = std::filesystem::exists(filepath);

            // рескан нужно делать только для реальных каталогов на диске
            if (filepathExists)
            {
                rescan_physical_path(P->m_Path, P->m_Flags.is(FS_Path::flRecurse));
            }

            P->m_Flags.set(FS_Path::flNeedRescan, FALSE);
        }
    }
}

void CLocatorAPI::lock_rescan() { m_iLockRescan++; }

void CLocatorAPI::unlock_rescan()
{
    m_iLockRescan--;
    VERIFY(m_iLockRescan >= 0);
    if ((0 == m_iLockRescan) && m_Flags.is(flNeedRescan))
        rescan_physical_pathes();
}

void CLocatorAPI::check_pathes()
{
    if (m_Flags.is(flNeedRescan) && (0 == m_iLockRescan))
    {
        lock_rescan();
        rescan_physical_pathes();
        unlock_rescan();
    }
}
