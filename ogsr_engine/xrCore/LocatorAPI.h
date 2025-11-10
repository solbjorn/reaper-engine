// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <io.h>

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

        gsl::index vfs; // VFS_STANDARD_FILE - standart file
        s64 size_real;

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

        s64 modif; // for editor
        bool folder;
    };

private:
    static constexpr gsl::index VFS_STANDARD_FILE{-1};
    static constexpr gsl::index BIG_FILE_READER_WINDOW_SIZE{1024 * 1024};

    struct file_pred
    {
        [[nodiscard]] constexpr bool operator()(const file& x, const file& y) const { return xr_strcmp(x.name, y.name) < 0; }
    };

    struct archive
    {
        shared_str path;
        gsl::index vfs_idx{VFS_STANDARD_FILE};
        s64 size{};

    private:
        class xr_sqfs_stream;
        struct xr_sqfs;

        enum class container
        {
            STANDARD,
            SQFS,
            DB,
        };
        container type{container::STANDARD};

        union
        {
            // SquashFS
            struct
            {
                sqfs_file_t* file{};
                sqfs_compressor_t* cmp{};
                xr_sqfs* fs{};
            };

            // DB
            struct
            {
                void* hSrcMap;
                CInifile* header;
                u32 key;
            };
        };

        void* hSrcFile{};

    public:
        archive() = default;

        // Implementation wrappers
        void open();
        [[nodiscard]] IC bool autoload();
        [[nodiscard]] IC const char* entry_point() const;
        IC void index(CLocatorAPI& loc, const char* fs_entry_point) const;
        [[nodiscard]] IC IReader* read(const char* fname, const struct file& desc, u32 gran) const;
        [[nodiscard]] IC CStreamReader* stream(const char* fname, const struct file& desc) const;
        IC void cleanup();
        void close();

    private:
        // SquashFS
        void open_sqfs();
        [[nodiscard]] bool autoload_sqfs();
        [[nodiscard]] const char* entry_point_sqfs() const;
        void index_dir_sqfs(CLocatorAPI& loc, const char* path, sqfs_dir_iterator_t* it) const;
        void index_sqfs(CLocatorAPI& loc, const char* fs_entry_point) const;
        [[nodiscard]] IReader* read_sqfs(const char*, const struct file& desc, u32) const;
        [[nodiscard]] CStreamReader* stream_sqfs(const char*, const struct file& desc) const;
        void cleanup_sqfs();
        void close_sqfs();

        // DB
        void open_db();
        [[nodiscard]] bool autoload_db();
        [[nodiscard]] const char* entry_point_db() const;
        void index_db(CLocatorAPI& loc, const char* entry_point) const;
        [[nodiscard]] IReader* read_db(const char* fname, const struct file& desc, u32 gran) const;
        [[nodiscard]] CStreamReader* stream_db(const char* fname, const struct file& desc) const;
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

    void Register(LPCSTR name, gsl::index vfs, u32 ptr, s64 size_real, u32 size_compressed, s64 modif);
    void LoadArchive(archive& A);
    void ProcessArchive(LPCSTR path);
    void ProcessOne(LPCSTR path, const _FINDDATA_T& F, bool bNoRecurse);
    [[nodiscard]] bool RecurseScanPhysicalPath(const char* path, const bool log_if_found, bool bNoRecurse);

    [[nodiscard]] files_it file_find_it(LPCSTR n);

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
    void file_from_cache_impl(IReader*& R, LPSTR fname, const file&);
    void file_from_cache_impl(CStreamReader*& R, LPSTR fname, const file&);

    template <typename T>
    void file_from_cache(T*& R, LPSTR fname, const file& desc, LPCSTR&);

    void file_from_archive(IReader*& R, LPCSTR fname, const file& desc);
    void file_from_archive(CStreamReader*& R, LPCSTR fname, const file& desc);

    [[nodiscard]] bool check_for_file(LPCSTR path, LPCSTR _fname, string_path& fname, const file*& desc);

    template <typename T>
    [[nodiscard]] IC T* r_open_impl(LPCSTR path, LPCSTR _fname);

public:
    CLocatorAPI();
    ~CLocatorAPI();

    void _initialize(u32 flags, LPCSTR target_folder = nullptr, LPCSTR fs_name = nullptr);
    void _destroy();

    [[nodiscard]] CStreamReader* rs_open(LPCSTR initial, LPCSTR N);
    [[nodiscard]] IReader* r_open(LPCSTR initial, LPCSTR N);
    [[nodiscard]] IC IReader* r_open(LPCSTR N) { return r_open(nullptr, N); }
    void r_close(IReader*& S);
    void r_close(CStreamReader*& fs);

    [[nodiscard]] IWriter* w_open(LPCSTR initial, LPCSTR N);
    [[nodiscard]] IC IWriter* w_open(LPCSTR N) { return w_open(nullptr, N); }
    [[nodiscard]] IWriter* w_open_ex(LPCSTR initial, LPCSTR N);
    [[nodiscard]] IC IWriter* w_open_ex(LPCSTR N) { return w_open_ex(nullptr, N); }
    void w_close(IWriter*& S);

    [[nodiscard]] const file* exist(LPCSTR N);
    [[nodiscard]] const file* exist(LPCSTR path, LPCSTR name);
    [[nodiscard]] const file* exist(string_path& fn, LPCSTR path, LPCSTR name);
    [[nodiscard]] const file* exist(string_path& fn, LPCSTR path, LPCSTR name, LPCSTR ext);

    void file_delete(LPCSTR path, LPCSTR nm);
    void file_delete(LPCSTR full_path) { file_delete(nullptr, full_path); }
    void file_copy(LPCSTR src, LPCSTR dest);
    void file_rename(LPCSTR src, LPCSTR dest, bool bOwerwrite = true);
    [[nodiscard]] s64 file_length(LPCSTR src);

    [[nodiscard]] s64 get_file_age(LPCSTR nm);

    [[nodiscard]] xr_vector<LPSTR>* file_list_open(LPCSTR initial, LPCSTR folder, u32 flags = FS_ListFiles);
    [[nodiscard]] xr_vector<LPSTR>* file_list_open(LPCSTR path, u32 flags = FS_ListFiles);
    void file_list_close(xr_vector<LPSTR>*& lst);

    [[nodiscard]] bool path_exist(LPCSTR path);
    [[nodiscard]] FS_Path* get_path(LPCSTR path);
    [[nodiscard]] FS_Path* append_path(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive);
    [[nodiscard]] LPCSTR update_path(string_path& dest, LPCSTR initial, LPCSTR src);

    [[nodiscard]] gsl::index file_list(FS_FileSet& dest, LPCSTR path, u32 flags = FS_ListFiles, LPCSTR mask = nullptr);

    // editor functions
    void rescan_physical_pathes();
    void lock_rescan();
    void unlock_rescan();
};

extern std::unique_ptr<CLocatorAPI> xr_FS;
#define FS (*xr_FS)

void _dump_open_files(int mode);
