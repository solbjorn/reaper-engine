#include "stdafx.h"

#include "LocatorAPI.h"

#include "FS_internal.h"
#include "stream_reader.h"

#include <bitmap_object_pool.hpp>

namespace sqfs
{
#define SQFS_STATIC

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <sqfs/compressor.h>
#include <sqfs/data_reader.h>
#include <sqfs/dir_entry.h>
#include <sqfs/dir_reader.h>
#include <sqfs/id_table.h>
#include <sqfs/inode.h>
#include <sqfs/io.h>

XR_DIAG_POP();

#undef SQFS_STATIC
} // namespace sqfs

template <>
struct std::default_delete<sqfs::sqfs_compressor_t>
{
    constexpr void operator()(sqfs::sqfs_compressor_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_data_reader_t>
{
    constexpr void operator()(sqfs::sqfs_data_reader_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_dir_entry_t>
{
    constexpr void operator()(sqfs::sqfs_dir_entry_t* ptr) const noexcept { sqfs::sqfs_free(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_dir_iterator_t>
{
    constexpr void operator()(sqfs::sqfs_dir_iterator_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_dir_reader_t>
{
    constexpr void operator()(sqfs::sqfs_dir_reader_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_file_t>
{
    constexpr void operator()(sqfs::sqfs_file_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_id_table_t>
{
    constexpr void operator()(sqfs::sqfs_id_table_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_inode_generic_t>
{
    constexpr void operator()(sqfs::sqfs_inode_generic_t* ptr) const noexcept { sqfs::sqfs_free(ptr); }
};

namespace xr
{
namespace
{
class sqfs_pool final
{
public:
    class reader final
    {
    public:
        std::unique_ptr<sqfs::sqfs_file_t> file;
        std::unique_ptr<sqfs::sqfs_compressor_t> cmp;
        std::unique_ptr<sqfs::sqfs_dir_reader_t> dr;
        std::unique_ptr<sqfs::sqfs_data_reader_t> data;

        reader() = delete;
        explicit reader(xr::sqfs_pool& sq);
    };

private:
    class pool_t final : public tzcnt_utils::BitmapObjectPoolImpl<reader, pool_t>
    {
        friend class tzcnt_utils::BitmapObjectPoolImpl<reader, pool_t>;

        void initialize(void* location, xr::sqfs_pool& sq) { new (location) reader{sq}; }
    };

    pool_t pool;

public:
    gsl::czstring path;
    sqfs::sqfs_super_t super;
    sqfs::sqfs_compressor_config_t cfg;

    sqfs_pool() = delete;
    explicit sqfs_pool(gsl::czstring path);

    [[nodiscard]] const auto acquire_scoped() { return pool.acquire_scoped(*this); }
};

sqfs_pool::sqfs_pool(gsl::czstring path) : path{path}
{
    sqfs::sqfs_file_t* f;
    R_ASSERT(sqfs::sqfs_file_open(&f, path, sqfs::SQFS_FILE_OPEN_READ_ONLY) == 0);
    const auto file = absl::WrapUnique(f);

    R_ASSERT(sqfs::sqfs_super_read(&super, file.get()) == 0);
    R_ASSERT(sqfs::sqfs_compressor_config_init(&cfg, gsl::narrow<sqfs::SQFS_COMPRESSOR>(super.compression_id), super.block_size,
                                               sqfs::SQFS_COMP_FLAG_UNCOMPRESS) == 0);
}

sqfs_pool::reader::reader(xr::sqfs_pool& sq)
{
    sqfs::sqfs_file_t* f;
    R_ASSERT(sqfs::sqfs_file_open(&f, sq.path, sqfs::SQFS_FILE_OPEN_READ_ONLY) == 0);
    file = absl::WrapUnique(f);

    sqfs::sqfs_compressor_t* c;
    R_ASSERT(sqfs::sqfs_compressor_create(&sq.cfg, &c) == 0);
    cmp = absl::WrapUnique(c);

    dr = absl::WrapUnique(sqfs::sqfs_dir_reader_create(&sq.super, cmp.get(), file.get(), 0));
    data = absl::WrapUnique(sqfs::sqfs_data_reader_create(file.get(), sq.super.block_size, cmp.get(), 0));
    R_ASSERT(dr && data);

    R_ASSERT(sqfs::sqfs_data_reader_load_fragment_table(data.get(), &sq.super) == 0);
}

// Stream reader for a file inside SquashFS

class sqfs_stream final : public CStreamReader
{
    RTTI_DECLARE_TYPEINFO(xr::sqfs_stream, CStreamReader);

private:
    static constexpr std::array<gsl::index, 4> wnd{0, 1024, 4096, 16384};

    decltype(std::declval<xr::sqfs_pool>().acquire_scoped()) obj;
    std::unique_ptr<sqfs::sqfs_inode_generic_t> inode;

    xr_vector<std::byte> buf;

    const gsl::index fbase;
    const gsl::index fsize;

    gsl::index foff{};
    gsl::index last{};

    gsl::index bsize;
    gsl::index boff{};

    xr::sqfs_pool& sq;
    const sqfs::sqfs_u64 ref;

    [[nodiscard]] const auto& rd() const { return obj.value; }

public:
    sqfs_stream() = delete;
    explicit sqfs_stream(xr::sqfs_pool& sq, sqfs::sqfs_u64 inodein, gsl::index base, gsl::index sz);
    ~sqfs_stream() override = default;

    void close() override;

    [[nodiscard]] gsl::index elapsed() const override { return fsize - tell(); }
    [[nodiscard]] gsl::index length() const override { return fsize; }
    [[nodiscard]] gsl::index tell() const override { return foff + boff; }
    void seek(gsl::index offset) override { advance(offset - tell()); }

    void advance(gsl::index offset) override;
    void r(void* buffer, gsl::index buffer_size) override;
    [[nodiscard]] CStreamReader* open_chunk(u32 chunk_id) override;
};

sqfs_stream::sqfs_stream(xr::sqfs_pool& sq, sqfs::sqfs_u64 inodein, gsl::index base, gsl::index sz)
    : obj{sq.acquire_scoped()}, fbase{base}, fsize{sz}, sq{sq}, ref{inodein}
{
    sqfs::sqfs_inode_generic_t* in;
    R_ASSERT(sqfs::sqfs_dir_reader_get_inode(rd().dr.get(), ref, &in) == 0);
    inode = absl::WrapUnique(in);

    bsize = std::min(wnd[3], fsize);
    buf.reserve(gsl::narrow_cast<size_t>(bsize));
}

void sqfs_stream::close()
{
    auto self = this;
    xr_delete(self);
}

void sqfs_stream::advance(gsl::index offset)
{
    const auto noff = boff + offset;
    if (noff >= 0 && noff < bsize)
    {
        boff = noff;
        return;
    }

    foff += noff;

    const auto woff = xr::rounddown(foff, wnd[3]);
    boff = foff - woff;
    foff = woff;
}

void sqfs_stream::r(void* buffer, gsl::index buffer_size)
{
    auto out = std::span{static_cast<std::byte*>(buffer), gsl::narrow_cast<size_t>(std::min(buffer_size, elapsed()))};

    while (!out.empty())
    {
        if (foff != last)
            buf.resize(wnd[0]);

        auto read = std::ssize(buf);
        if (read == bsize)
            goto copy;

        if (boff == 0 && read == wnd[0])
        {
            const auto direct = xr::rounddown(std::ssize(out), wnd[3]);
            if (direct > 0)
            {
                R_ASSERT(sqfs::sqfs_data_reader_read(rd().data.get(), inode.get(), gsl::narrow_cast<sqfs::sqfs_u64>(fbase + foff), &out[0],
                                                     gsl::narrow_cast<sqfs::sqfs_u32>(direct)) == direct);

                out = out.subspan(gsl::narrow_cast<size_t>(direct));
                last = foff;
                advance(direct);

                if (out.empty())
                    return;
            }
        }

        if (const auto end = boff + std::ssize(out); end > read)
        {
            gsl::index precache;

            if (read == wnd[0] && fsize <= wnd[3])
                precache = fsize;
            else if (read == wnd[0] && end <= wnd[1])
                precache = wnd[1];
            else if (read <= wnd[1] && end <= wnd[2])
                precache = wnd[2] - read;
            else
                precache = wnd[3] - read;

            buf.resize(gsl::narrow_cast<size_t>(read + precache));
            precache = std::min(precache, fsize - foff - read);

            R_ASSERT(sqfs::sqfs_data_reader_read(rd().data.get(), inode.get(), gsl::narrow_cast<sqfs::sqfs_u64>(fbase + foff + read),
                                                 &buf[gsl::narrow_cast<size_t>(read)], gsl::narrow_cast<sqfs::sqfs_u32>(precache)) == precache);
            read += precache;
        }

    copy:
        const auto copy = std::min(std::ssize(out), read - boff);
        std::memcpy(&out[0], &buf[gsl::narrow_cast<size_t>(boff)], gsl::narrow_cast<size_t>(copy));

        out = out.subspan(gsl::narrow_cast<size_t>(copy));
        last = foff;
        advance(copy);
    }
}

CStreamReader* sqfs_stream::open_chunk(u32 chunk_id)
{
    ::BOOL compressed;

    const auto size = find_chunk(chunk_id, &compressed);
    if (size == 0)
        return nullptr;

    R_ASSERT(!compressed, "cannot stream compressed chunks");

    return xr_new<xr::sqfs_stream>(sq, ref, tell(), size);
}

[[nodiscard]] constexpr auto regular(auto mode) { return (mode & sqfs::SQFS_INODE_MODE_MASK) == sqfs::SQFS_INODE_MODE_REG; }

static_assert(sizeof(std::unique_ptr<xr::sqfs_pool>) == sizeof(uintptr_t));

[[nodiscard]] constexpr auto& sqfs_cb(uintptr_t& cb) { return *reinterpret_cast<std::unique_ptr<xr::sqfs_pool>*>(&cb); }
[[nodiscard]] constexpr const auto& sqfs_cb(const uintptr_t& cb) { return *reinterpret_cast<const std::unique_ptr<xr::sqfs_pool>*>(&cb); }
} // namespace
} // namespace xr

// SquashFS

void CLocatorAPI::archive::open_sqfs() { xr::sqfs_cb(cb) = std::make_unique<xr::sqfs_pool>(path.c_str()); }

void CLocatorAPI::archive::autoload_sqfs()
{
    // SquashFS doesn't have metadata, always autoload archives
}

void CLocatorAPI::archive::index_sqfs(CLocatorAPI& loc, gsl::czstring fs_entry_point) const
{
    auto obj = xr::sqfs_cb(cb)->acquire_scoped();
    const auto& rd = obj.value;

    const auto idt = absl::WrapUnique(sqfs::sqfs_id_table_create(0));
    R_ASSERT(idt && sqfs::sqfs_id_table_read(idt.get(), rd.file.get(), &xr::sqfs_cb(cb)->super, rd.cmp.get()) == 0);

    const auto it = [&rd, &idt] [[nodiscard]] {
        sqfs::sqfs_inode_generic_t* inp;
        R_ASSERT(sqfs::sqfs_dir_reader_get_root_inode(rd.dr.get(), &inp) == 0);
        const auto inode = absl::WrapUnique(inp);

        sqfs::sqfs_dir_iterator_t* itp;
        R_ASSERT(sqfs::sqfs_dir_iterator_create(rd.dr.get(), idt.get(), rd.data.get(), nullptr, inode.get(), &itp) == 0);
        const auto it = absl::WrapUnique(itp);

        sqfs::sqfs_dir_iterator_t* rec;
        R_ASSERT(sqfs::sqfs_dir_iterator_create_recursive(&rec, it.get()) == 0);
        return absl::WrapUnique(rec);
    }();

    for (;;)
    {
        sqfs::sqfs_dir_entry_t* ep;

        if (const auto ret = it->next(it.get(), &ep); ret != 0)
        {
            R_ASSERT(ret == 1);
            return;
        }

        const auto ent = absl::WrapUnique(ep);
        if (!xr::regular(ent->mode))
            continue;

        loc.Register(fs_entry_point, ent->name, vfs_idx, ent->inode, gsl::narrow<s64>(ent->size), ent->mtime);
    }
}

IReader* CLocatorAPI::archive::read_sqfs(const struct file& desc, u32) const
{
    auto obj = xr::sqfs_cb(cb)->acquire_scoped();
    const auto& rd = obj.value;

    sqfs::sqfs_inode_generic_t* inp;
    R_ASSERT(sqfs::sqfs_dir_reader_get_inode(rd.dr.get(), desc.cb, &inp) == 0);
    const auto inode = absl::WrapUnique(inp);

    std::byte* dest = xr_alloc<std::byte>(desc.size_real);
    R_ASSERT(sqfs::sqfs_data_reader_read(rd.data.get(), inode.get(), 0, dest, gsl::narrow_cast<sqfs::sqfs_u32>(desc.size_real)) == desc.size_real);

    return xr_new<CTempReader>(dest, desc.size_real, 0z);
}

CStreamReader* CLocatorAPI::archive::stream_sqfs(const struct file& desc) const
{
    return xr_new<xr::sqfs_stream>(*xr::sqfs_cb(cb), desc.cb, 0, desc.size_real);
}

void CLocatorAPI::archive::close_sqfs() { xr::sqfs_cb(cb).reset(); }
