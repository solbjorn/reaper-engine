#include "stdafx.h"

#include "LocatorAPI.h"

#include "FS_internal.h"
#include "stream_reader.h"
#include "trivial_encryptor.h"

namespace xr
{
namespace detail
{
class opaque_mapping;
}

namespace
{
// 2947 format support is limited to the exclusions listed in this array
// (vanilla SoC RU archives), the rest must be in DwarFS, SquashFS, ZIP,
// a [lib]archive-backed format, or XDB.
constexpr struct
{
    std::string_view ext;
    u32 size;
    u32 key;
} excls[] = {
#define EXCL_RU(l, s) \
    { \
        .ext{".db" #l}, \
        .size = s, \
        .key = 1 + std::to_underlying(trivial_encryptor::key_flag::russian), \
    }
    EXCL_RU(0, 678577379), EXCL_RU(1, 708434331), EXCL_RU(2, 671169415), EXCL_RU(3, 684792231), EXCL_RU(4, 671392842),
    EXCL_RU(5, 696277331), EXCL_RU(6, 667613134), EXCL_RU(7, 672770451), EXCL_RU(8, 296000567), EXCL_RU(9, 79312307),
    EXCL_RU(a, 32500627),  EXCL_RU(b, 55194918),  EXCL_RU(c, 160948),    EXCL_RU(d, 101014)
#undef EXCL_RU
};
} // namespace
} // namespace xr

template <>
struct std::default_delete<xr::detail::opaque_mapping>
{
    constexpr void operator()(xr::detail::opaque_mapping* ptr) const noexcept { ::CloseHandle(ptr); }
};

namespace xr
{
namespace
{
IReader* open_chunk(void* ptr, u32 ID, gsl::czstring archiveName, s64 archiveSize, u32 key = 0)
{
    u32 dwType = INVALID_SET_FILE_POINTER;
    unsigned long read_byte{};
    gsl::index dwSize{};

    u32 dwPtr = ::SetFilePointer(ptr, 0, nullptr, FILE_BEGIN);
    R_ASSERT3(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));

    while (true)
    {
        bool res = ::ReadFile(ptr, &dwType, 4, &read_byte, nullptr);
        R_ASSERT3(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));

        u32 tempSize = 0;
        res = ::ReadFile(ptr, &tempSize, 4, &read_byte, nullptr);
        R_ASSERT3(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));
        dwSize = tempSize;

        if ((dwType & ~CFS_CompressMark) == ID)
        {
            std::byte* src_data = xr_alloc<std::byte>(dwSize);
            res = ::ReadFile(ptr, src_data, gsl::narrow_cast<u32>(dwSize), &read_byte, nullptr);
            R_ASSERT3(res && read_byte == dwSize, archiveName, Debug.error2string(GetLastError()));

            if (dwType & CFS_CompressMark)
            {
                ::BYTE* dest{};
                gsl::index dest_sz{};

                if (key != 0)
                    g_trivial_encryptor.decode(src_data, dwSize, src_data, gsl::narrow<trivial_encryptor::key_flag>(key - 1));

                bool result = _decompressLZ(&dest, &dest_sz, src_data, dwSize, archiveSize);
                CHECK_OR_EXIT(result, xr::format("[{}] Can't decompress archive [{}]", __FUNCTION__, archiveName));
                xr_free(src_data);

                return xr_new<CTempReader>(dest, dest_sz, 0z);
            }
            else
            {
                return xr_new<CTempReader>(src_data, dwSize, 0z);
            }
        }
        else
        {
            dwPtr = ::SetFilePointer(ptr, gsl::narrow<s32>(dwSize), nullptr, FILE_CURRENT);
            R_ASSERT3(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));
        }
    }
}

static_assert(sizeof(std::unique_ptr<xr::detail::opaque_mapping>) == sizeof(uintptr_t));

[[nodiscard]] constexpr auto& db_cb(uintptr_t& cb) { return *reinterpret_cast<std::unique_ptr<xr::detail::opaque_mapping>*>(&cb); }
[[nodiscard]] constexpr const auto& db_cb(const uintptr_t& cb) { return *reinterpret_cast<const std::unique_ptr<xr::detail::opaque_mapping>*>(&cb); }
} // namespace
} // namespace xr

void CLocatorAPI::archive::open_db()
{
    auto map = ::CreateFileMapping(hSrcFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    R_ASSERT(map != INVALID_HANDLE_VALUE);

    xr::db_cb(cb).reset(static_cast<xr::detail::opaque_mapping*>(map));
}

void CLocatorAPI::archive::autoload_db()
{
    for (const auto& excl : xr::excls)
    {
        if (std::is_eq(xr_strcmp(strext(*path), excl.ext)) && size == excl.size)
        {
            key = excl.key;
            break;
        }
    }

    IReader* hdr = key == 0 ? xr::open_chunk(hSrcFile, CFS_HeaderChunkID, path.c_str(), size) : nullptr;
    if (hdr == nullptr)
        return;

    header = std::make_unique<CInifile>(hdr, path.c_str());
    hdr->close();
}

void CLocatorAPI::archive::index_db(CLocatorAPI& loc, gsl::czstring fs_entry_point) const
{
    IReader* hdr = xr::open_chunk(hSrcFile, 1, path.c_str(), size, key);
    R_ASSERT(hdr);

    while (!hdr->eof())
    {
        std::array<std::byte, 1024> buffer_start;
        string_path name;

        gsl::index buffer_size = hdr->r_u16();
        VERIFY(buffer_size < sizeof(name) + 4 * sizeof(u32));
        VERIFY(buffer_size < sizeof(buffer_start));

        auto buffer{buffer_start.begin()};
        hdr->r(&*buffer, buffer_size);

        u32 size_real = *reinterpret_cast<u32*>(&*buffer);
        buffer += sizeof(size_real);

        // Compressed size, must be equal to the real size
        R_ASSERT(*reinterpret_cast<u32*>(&*buffer) == size_real);
        buffer += sizeof(u32);

        // Skip unused checksum
        buffer += sizeof(u32);

        gsl::index name_length = buffer_size - gsl::index{4 * sizeof(u32)};
        std::memcpy(name, &*buffer, gsl::narrow_cast<size_t>(name_length));
        name[name_length] = '\0';
        buffer += buffer_size - gsl::index{4 * sizeof(u32)};

        u32 ptr = *reinterpret_cast<u32*>(&*buffer);
        buffer += sizeof(ptr);

        loc.Register(fs_entry_point, &name[0], vfs_idx, ptr, size_real, 0);
    }

    hdr->close();
}

IReader* CLocatorAPI::archive::read_db(const struct file& desc, u32 gran) const
{
    const auto desc_ptr = gsl::narrow_cast<u32>(desc.cb);
    const gsl::index start = (desc_ptr / gran) * gran;

    gsl::index end = (desc_ptr + desc.size_real) / gran;
    if ((desc_ptr + desc.size_real) % gran)
        end += 1;

    end *= gran;
    if (end > size)
        end = size;

    const gsl::index sz = end - start;

    auto ptr = static_cast<std::byte*>(::MapViewOfFile(xr::db_cb(cb).get(), FILE_MAP_READ, 0, gsl::narrow<unsigned long>(start), gsl::narrow_cast<size_t>(sz)));
    if (!ptr)
    {
        VERIFY3(ptr, "cannot create file mapping on file", desc.name);
        return nullptr;
    }

#ifdef DEBUG
    string512 temp;
    sprintf_s(temp, "%s:%s", path.c_str(), desc.name);

    register_file_mapping(ptr, sz, temp);
#endif // DEBUG

    const gsl::index ptr_offs = desc_ptr - start;

    return xr_new<CPackReader>(ptr, ptr + ptr_offs, desc.size_real);
}

CStreamReader* CLocatorAPI::archive::stream_db(const struct file& desc) const
{
    CMapStreamReader* R = xr_new<CMapStreamReader>();
    R->construct(xr::db_cb(cb).get(), gsl::narrow_cast<u32>(desc.cb), desc.size_real, size, BIG_FILE_READER_WINDOW_SIZE);
    return R;
}

void CLocatorAPI::archive::close_db() { xr::db_cb(cb).reset(); }
