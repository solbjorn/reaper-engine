#include "stdafx.h"

#include "FS_internal.h"
#include "stream_reader.h"
#include "trivial_encryptor.h"

// 2947 format support is limited to the exclusions listed in this array
// (vanilla SoC RU archives), the rest must be in either SquashFS or XDB
static const struct
{
    const char* ext;
    u32 size;
    u32 key;
} excls[] = {
#define EXCL_RU(l, s) \
    { \
        .ext = ".db" #l, \
        .size = (s), \
        .key = 1 + (u32)trivial_encryptor::key_flag::russian, \
    }
    EXCL_RU(0, 678577379), EXCL_RU(1, 708434331), EXCL_RU(2, 671169415), EXCL_RU(3, 684792231), EXCL_RU(4, 671392842),
    EXCL_RU(5, 696277331), EXCL_RU(6, 667613134), EXCL_RU(7, 672770451), EXCL_RU(8, 296000567), EXCL_RU(9, 79312307),
    EXCL_RU(a, 32500627),  EXCL_RU(b, 55194918),  EXCL_RU(c, 160948),    EXCL_RU(d, 101014)
#undef EXCL_RU
};

static IReader* open_chunk(void* ptr, u32 ID, const char* archiveName, size_t archiveSize, const u32 key = 0)
{
    u32 dwType = INVALID_SET_FILE_POINTER;
    size_t dwSize = 0;
    DWORD read_byte;
    u32 dwPtr = SetFilePointer(ptr, 0, nullptr, FILE_BEGIN);
    R_ASSERT3(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));
    while (true)
    {
        bool res = ReadFile(ptr, &dwType, 4, &read_byte, nullptr);
        R_ASSERT3(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));

        u32 tempSize = 0;
        res = ReadFile(ptr, &tempSize, 4, &read_byte, nullptr);
        dwSize = tempSize;
        R_ASSERT3(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));

        if ((dwType & ~CFS_CompressMark) == ID)
        {
            u8* src_data = xr_alloc<u8>(dwSize);
            res = ReadFile(ptr, src_data, dwSize, &read_byte, nullptr);
            R_ASSERT3(res && read_byte == dwSize, archiveName, Debug.error2string(GetLastError()));
            if (dwType & CFS_CompressMark)
            {
                BYTE* dest{};
                size_t dest_sz{};

                if (key)
                    g_trivial_encryptor.decode(src_data, dwSize, src_data, (trivial_encryptor::key_flag)(key - 1));

                bool result = _decompressLZ(&dest, &dest_sz, src_data, dwSize, archiveSize);
                CHECK_OR_EXIT(result, make_string("[%s] Can't decompress archive [%s]", __FUNCTION__, archiveName));

                xr_free(src_data);
                return xr_new<CTempReader>(dest, dest_sz, 0uz);
            }
            else
            {
                return xr_new<CTempReader>(src_data, dwSize, 0uz);
            }
        }
        else
        {
            dwPtr = SetFilePointer(ptr, dwSize, nullptr, FILE_CURRENT);
            R_ASSERT3(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));
        }
    }
    return nullptr;
}

void CLocatorAPI::archive::open_db()
{
    type = container::DB;

    hSrcMap = CreateFileMapping(hSrcFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    R_ASSERT(hSrcMap != INVALID_HANDLE_VALUE);
}

bool CLocatorAPI::archive::autoload_db()
{
    for (const auto& excl : excls)
    {
        if (std::is_eq(xr_strcmp(strext(*path), excl.ext)) && size == excl.size)
        {
            key = excl.key;
            break;
        }
    }

    // Read header
    bool load = true;

    IReader* hdr = !key ? open_chunk(hSrcFile, CFS_HeaderChunkID, path.c_str(), size) : nullptr;
    if (hdr)
    {
        header = xr_new<CInifile>(hdr, "archive_header");
        hdr->close();
        load = header->r_bool("header", "auto_load");
    }

    return load;
}

const char* CLocatorAPI::archive::entry_point_db() { return header ? header->r_string("header", "entry_point") : nullptr; }

void CLocatorAPI::archive::index_db(CLocatorAPI& loc, const char* fs_entry_point)
{
    IReader* hdr = open_chunk(hSrcFile, 1, path.c_str(), size, key);
    R_ASSERT(hdr);
    RStringVec fv;
    while (!hdr->eof())
    {
        string_path name, full;
        string1024 buffer_start;
        size_t buffer_size = hdr->r_u16();
        VERIFY(buffer_size < sizeof(name) + 4 * sizeof(u32));
        VERIFY(buffer_size < sizeof(buffer_start));
        u8* buffer = (u8*)&*buffer_start;
        hdr->r(buffer, buffer_size);

        u32 size_real = *(u32*)buffer;
        buffer += sizeof(size_real);

        u32 size_compr = *(u32*)buffer;
        buffer += sizeof(size_compr);

        // Skip unused checksum
        buffer += sizeof(u32);

        size_t name_length = buffer_size - 4 * sizeof(u32);
        Memory.mem_copy(name, buffer, name_length);
        name[name_length] = 0;
        buffer += buffer_size - 4 * sizeof(u32);

        R_ASSERT2(size_compr == size_real, make_string("error indexing %s\\%s: per-file compression support is deprecated and was removed", *path, name));

        u32 ptr = *(u32*)buffer;
        buffer += sizeof(ptr);

        strconcat(sizeof(full), full, fs_entry_point, name);

        loc.Register(full, vfs_idx, ptr, size_real, size_compr, 0);
    }
    hdr->close();
}

IReader* CLocatorAPI::archive::read_db(const char* fname, const struct file& desc, u32 gran)
{
    size_t start = (desc.ptr / gran) * gran;
    size_t end = (desc.ptr + desc.size_compressed) / gran;
    if ((desc.ptr + desc.size_compressed) % gran)
        end += 1;
    end *= gran;
    if (end > size)
        end = size;
    size_t sz = end - start;

    u8* ptr = (u8*)MapViewOfFile(hSrcMap, FILE_MAP_READ, 0, start, sz);
    if (!ptr)
    {
        VERIFY3(ptr, "cannot create file mapping on file", fname);
        return nullptr;
    }

#ifdef DEBUG
    string512 temp;
    sprintf_s(temp, "%s:%s", *path, fname);

    register_file_mapping(ptr, sz, temp);
#endif // DEBUG

    size_t ptr_offs = desc.ptr - start;

    return xr_new<CPackReader>(ptr, ptr + ptr_offs, desc.size_real);
}

CStreamReader* CLocatorAPI::archive::stream_db(const char* fname, const struct file& desc)
{
    R_ASSERT2(desc.size_compressed == desc.size_real, make_string("cannot use stream reading for compressed data %s, do not compress data to be streamed", fname));

    CMapStreamReader* R = xr_new<CMapStreamReader>();
    R->construct(hSrcMap, desc.ptr, desc.size_compressed, size, BIG_FILE_READER_WINDOW_SIZE);
    return R;
}

void CLocatorAPI::archive::cleanup_db()
{
    xr_delete(header);
    header = nullptr;
}

void CLocatorAPI::archive::close_db()
{
    CloseHandle(hSrcMap);
    hSrcMap = nullptr;
}
