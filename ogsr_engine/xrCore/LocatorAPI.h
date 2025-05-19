// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(push)
#pragma warning(disable : 4995)
#include <io.h>
#pragma warning(pop)

#include "LocatorAPI_defs.h"

class CStreamReader;

struct sqfs_compressor_t;
struct sqfs_dir_iterator_t;
struct sqfs_file_t;

class CLocatorAPI
{
    friend class FS_Path;

public:
    struct file
    {
        const char* name; // low-case name
        size_t vfs; // VFS_STANDARD_FILE - standart file

        u32 size_real;
        u32 folder : 1;

        union
        {
            // SquashFS
            u64 inode;

            // DB
            struct
            {
                u32 ptr; // pointer inside vfs
                u32 size_compressed; // if (size_real==size_compressed) - uncompressed
            };
        };

        u32 modif; // for editor
    };

private:
    static constexpr size_t VFS_STANDARD_FILE = std::numeric_limits<size_t>::max();
    static constexpr size_t BIG_FILE_READER_WINDOW_SIZE = 1024 * 1024;

    struct file_pred
    {
        IC bool operator()(const file& x, const file& y) const { return xr_strcmp(x.name, y.name) < 0; }
    };

    struct archive
    {
        shared_str path;
        size_t vfs_idx;
        size_t size;

    private:
        class xr_sqfs_stream;
        struct xr_sqfs;

        enum container
        {
            STANDARD,
            SQFS,
            DB,
        };
        container type : 3;

        union
        {
            // SquashFS
            struct
            {
                sqfs_file_t* file;
                sqfs_compressor_t* cmp;
                xr_sqfs* fs;
            };

            // DB
            struct
            {
                void* hSrcMap;
                CInifile* header;
                u32 key;
            };
        };

        void* hSrcFile;

    public:
        archive() : hSrcFile(NULL), vfs_idx(VFS_STANDARD_FILE), size(0), type(container::STANDARD), file(nullptr), cmp(nullptr), fs(nullptr) {}

        // Implementation wrappers
        void open();
        IC bool autoload();
        IC const char* entry_point();
        IC void index(CLocatorAPI& loc, const char* fs_entry_point);
        IC IReader* read(const char* fname, const struct file& desc, u32 gran);
        IC CStreamReader* stream(const char* fname, const struct file& desc);
        IC void cleanup();
        void close();

    private:
        // SquashFS
        void open_sqfs();
        bool autoload_sqfs();
        const char* entry_point_sqfs();
        void index_dir_sqfs(CLocatorAPI& loc, const char* path, sqfs_dir_iterator_t* it);
        void index_sqfs(CLocatorAPI& loc, const char* fs_entry_point);
        IReader* read_sqfs(const char* fname, const struct file& desc, u32 gran);
        CStreamReader* stream_sqfs(const char* fname, const struct file& desc);
        void cleanup_sqfs();
        void close_sqfs();

        // DB
        void open_db();
        bool autoload_db();
        const char* entry_point_db();
        void index_db(CLocatorAPI& loc, const char* entry_point);
        IReader* read_db(const char* fname, const struct file& desc, u32 gran);
        CStreamReader* stream_db(const char* fname, const struct file& desc);
        void cleanup_db();
        void close_db();
    };

    DEFINE_MAP_PRED(LPCSTR, FS_Path*, PathMap, PathPairIt, pred_str);
    PathMap pathes;

    DEFINE_SET_PRED(file, files_set, files_it, file_pred);
    DEFINE_VECTOR(archive, archives_vec, archives_it);

    int m_iLockRescan;

    void rescan_physical_path(LPCSTR full_path, BOOL bRecurse);
    void check_pathes();

    files_set files;
    archives_vec archives;

    const CLocatorAPI::file* Register(LPCSTR name, size_t vfs, u32 ptr, u32 size_real, u32 size_compressed, u32 modif);
    void LoadArchive(archive& A);
    void ProcessArchive(LPCSTR path);
    void ProcessOne(LPCSTR path, const _finddata_t& F, bool bNoRecurse);
    bool RecurseScanPhysicalPath(const char* path, const bool log_if_found, bool bNoRecurse);

    files_it file_find_it(LPCSTR n);

public:
    enum
    {
        flNeedRescan = (1 << 0),
        flBuildCopy = (1 << 1),
        flReady = (1 << 2),
        flEBuildCopy = (1 << 3),
        flEventNotificator = (1 << 4),
        flTargetFolderOnly = (1 << 5),
        flCacheFiles = (1 << 6),
        flScanAppRoot = (1 << 7),
        flNeedCheck = (1 << 8),
        flDumpFileActivity = (1 << 9),
    };
    Flags32 m_Flags;
    u32 dwAllocGranularity;
    u32 dwOpenCounter;

private:
    void file_from_cache_impl(IReader*& R, LPSTR fname, const file& desc);
    void file_from_cache_impl(CStreamReader*& R, LPSTR fname, const file& desc);

    template <typename T>
    void file_from_cache(T*& R, LPSTR fname, const file& desc, LPCSTR& source_name);

    void file_from_archive(IReader*& R, LPCSTR fname, const file& desc);
    void file_from_archive(CStreamReader*& R, LPCSTR fname, const file& desc);

    bool check_for_file(LPCSTR path, LPCSTR _fname, string_path& fname, const file*& desc);

    template <typename T>
    IC T* r_open_impl(LPCSTR path, LPCSTR _fname);

public:
    CLocatorAPI();
    ~CLocatorAPI();
    void _initialize(u32 flags, LPCSTR target_folder = 0, LPCSTR fs_name = 0);
    void _destroy();

    CStreamReader* rs_open(LPCSTR initial, LPCSTR N);
    IReader* r_open(LPCSTR initial, LPCSTR N);
    IC IReader* r_open(LPCSTR N) { return r_open(0, N); }
    void r_close(IReader*& S);
    void r_close(CStreamReader*& fs);

    IWriter* w_open(LPCSTR initial, LPCSTR N);
    IC IWriter* w_open(LPCSTR N) { return w_open(0, N); }
    IWriter* w_open_ex(LPCSTR initial, LPCSTR N);
    IC IWriter* w_open_ex(LPCSTR N) { return w_open_ex(0, N); }
    void w_close(IWriter*& S);

    const file* exist(LPCSTR N);
    const file* exist(LPCSTR path, LPCSTR name);
    const file* exist(string_path& fn, LPCSTR path, LPCSTR name);
    const file* exist(string_path& fn, LPCSTR path, LPCSTR name, LPCSTR ext);

    void file_delete(LPCSTR path, LPCSTR nm);
    void file_delete(LPCSTR full_path) { file_delete(0, full_path); }
    void file_copy(LPCSTR src, LPCSTR dest);
    void file_rename(LPCSTR src, LPCSTR dest, bool bOwerwrite = true);
    int file_length(LPCSTR src);

    u32 get_file_age(LPCSTR nm);

    xr_vector<LPSTR>* file_list_open(LPCSTR initial, LPCSTR folder, u32 flags = FS_ListFiles);
    xr_vector<LPSTR>* file_list_open(LPCSTR path, u32 flags = FS_ListFiles);
    void file_list_close(xr_vector<LPSTR>*& lst);

    bool path_exist(LPCSTR path);
    FS_Path* get_path(LPCSTR path);
    FS_Path* append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive);
    LPCSTR update_path(string_path& dest, LPCSTR initial, LPCSTR src);

    size_t file_list(FS_FileSet& dest, LPCSTR path, u32 flags = FS_ListFiles, LPCSTR mask = 0);

    // editor functions
    void rescan_physical_pathes();
    void lock_rescan();
    void unlock_rescan();
};

extern std::unique_ptr<CLocatorAPI> xr_FS;
#define FS (*xr_FS)
