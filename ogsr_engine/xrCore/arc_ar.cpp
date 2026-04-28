#include "stdafx.h"

#include "LocatorAPI.h"

#include "FS_internal.h"
#include "stream_reader.h"

#include <bitmap_object_pool.hpp>

namespace ar
{
#define LIBARCHIVE_STATIC
#define dev_t ::_dev_t

#include <archive.h>
#include <archive_entry.h>

#undef dev_t
#undef LIBARCHIVE_STATIC
} // namespace ar

template <>
struct std::default_delete<ar::archive>
{
    constexpr void operator()(ar::archive* ptr) const noexcept { R_ASSERT(ar::archive_read_free(ptr) == ARCHIVE_OK); }
};

namespace xr
{
namespace
{
class ar_pool final
{
public:
    class reader final
    {
    private:
        static constexpr gsl::index wnd{16384};

        std::unique_ptr<ar::archive> arc;
        s32 fd;

    public:
        s32 pos;

    private:
        [[nodiscard]] static constexpr gsl::czstring passphrase(ar::archive*, void*) { return XR_ARC_PASSWORD; }

    public:
        reader() = delete;

        explicit reader(gsl::czstring path)
        {
            R_ASSERT(::_sopen_s(&fd, path, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD) == 0);
            reopen();
        }

        ~reader() { ::_close(fd); }

        [[nodiscard]] constexpr auto get() const { return arc.get(); }

    private:
        void reopen();

    public:
        void goto_entry(s32 cd_pos);
    };

private:
    class pool_t final : public tzcnt_utils::BitmapObjectPoolImpl<reader, pool_t>
    {
        friend class tzcnt_utils::BitmapObjectPoolImpl<reader, pool_t>;

        void initialize(void* location, gsl::czstring path) { new (location) reader{path}; }
    };

    pool_t pool;
    gsl::czstring path;

public:
    ar_pool() = delete;
    explicit ar_pool(gsl::czstring path) : path{path} {}

    [[nodiscard]] const auto acquire_scoped() { return pool.acquire_scoped(path); }
};

void ar_pool::reader::reopen()
{
    arc.reset(ar::archive_read_new());
    R_ASSERT(ar::archive_read_set_passphrase_callback(get(), this, &xr::ar_pool::reader::passphrase) == ARCHIVE_OK);

    R_ASSERT(ar::archive_read_support_filter_all(get()) == ARCHIVE_OK);
    R_ASSERT(ar::archive_read_support_format_all(get()) == ARCHIVE_OK);

    R_ASSERT(::_lseeki64(fd, 0, SEEK_SET) == 0);
    R_ASSERT(ar::archive_read_open_fd(get(), fd, wnd) == ARCHIVE_OK);

    pos = -1;
}

void ar_pool::reader::goto_entry(s32 cd_pos)
{
    if (pos >= cd_pos)
        reopen();

    for (++pos;; ++pos)
    {
        ar::archive_entry* entry;
        R_ASSERT(ar::archive_read_next_header(get(), &entry) == ARCHIVE_OK);

        if (pos == cd_pos)
            return;
    }

    NODEFAULT;
}

// Stream reader for a file inside [lib]archive

class ar_stream final : public CStreamReader
{
    RTTI_DECLARE_TYPEINFO(xr::ar_stream, CStreamReader);

private:
    class buf final : public xr_vector<std::byte>
    {
    public:
        gsl::index foff;
        gsl::index bsize;
    };

    static constexpr std::array<gsl::index, 6> wnd{0, 1024, 4096, 16384, 65536, 262144};

    decltype(std::declval<xr::ar_pool>().acquire_scoped()) obj;

    std::array<buf, 4> bufs;

    const gsl::index fbase;
    const gsl::index fsize;

    gsl::index foff{};
    gsl::index last{};

    gsl::index curr{};
    gsl::index boff{};

    gsl::index pos;

    xr::ar_pool& ar;
    const s64 cd_pos;

    [[nodiscard]] auto& rd() const { return obj.value; }

    void reopen();
    void skip(gsl::index len);
    void next();

public:
    ar_stream() = delete;
    explicit ar_stream(xr::ar_pool& ar, s64 cd_pos, gsl::index base, gsl::index sz);
    ~ar_stream() override = default;

    void close() override;

    [[nodiscard]] gsl::index elapsed() const override { return fsize - tell(); }
    [[nodiscard]] gsl::index length() const override { return fsize; }
    [[nodiscard]] gsl::index tell() const override { return foff + boff; }
    void seek(gsl::index offset) override { advance(offset - tell()); }

    void advance(gsl::index offset) override;
    void r(void* buffer, gsl::index buffer_size) override;
    [[nodiscard]] CStreamReader* open_chunk(u32 chunk_id) override;
};

ar_stream::ar_stream(xr::ar_pool& ar, s64 cd_pos, gsl::index base, gsl::index sz) : obj{ar.acquire_scoped()}, fbase{base}, fsize{sz}, ar{ar}, cd_pos{cd_pos}
{
    gsl::index start{0};
    auto left = fsize;

    for (auto& buf : bufs)
    {
        const auto bsize = std::min(wnd[5], left);

        buf.reserve(gsl::narrow_cast<size_t>(bsize));
        buf.foff = bsize > 0 ? start : 0;
        buf.bsize = bsize;

        start += bsize;
        left -= bsize;
    }

    reopen();
}

void ar_stream::reopen()
{
    rd().goto_entry(gsl::narrow_cast<s32>(cd_pos));

    if (fbase > 0)
        skip(fbase);

    pos = 0;
}

void ar_stream::close()
{
    auto self = this;
    xr_delete(self);
}

void ar_stream::advance(gsl::index offset)
{
    const auto noff = boff + offset;
    if (noff >= 0 && noff < bufs[gsl::narrow_cast<size_t>(curr)].bsize)
    {
        boff = noff;
        return;
    }

    foff += noff;

    const auto woff = xr::rounddown(foff, wnd[5]);
    boff = foff - woff;
    foff = woff;
}

void ar_stream::skip(gsl::index len)
{
    auto& buf = bufs[gsl::narrow_cast<size_t>(curr)];
    const auto bsize = std::min(buf.bsize, len);

    buf.resize(gsl::narrow_cast<size_t>(bsize));

    for (auto left = len; left > 0;)
    {
        const auto skip = std::min(bsize, left);

        R_ASSERT(ar::archive_read_data(rd().get(), &buf[0], gsl::narrow_cast<size_t>(skip)) == skip);
        left -= skip;
    }

    buf.resize(wnd[0]);
}

void ar_stream::next()
{
    auto buf = &bufs[gsl::narrow_cast<size_t>(curr)];
    auto read = std::ssize(*buf);

    if (read == wnd[0])
        goto pick;

    if (const auto left = std::min(buf->bsize - read, fsize - pos); left > 0)
    {
        buf->resize(gsl::narrow_cast<size_t>(buf->bsize));

        R_ASSERT(ar::archive_read_data(rd().get(), &(*buf)[gsl::narrow_cast<size_t>(read)], gsl::narrow_cast<size_t>(left)) == left);
        pos += left;
    }

pick:
    if (foff > 0)
    {
        if (const auto it = std::ranges::find(bufs, foff, &xr::ar_stream::buf::foff); it == bufs.end())
        {
            if ((curr == 0 || read > wnd[0]) && ++curr == std::ssize(bufs))
                curr = 1;

            buf = &bufs[gsl::narrow_cast<size_t>(curr)];
            buf->resize(wnd[0]);
            buf->foff = foff;
        }
        else
        {
            curr = it - bufs.begin();
        }
    }
    else
    {
        curr = 0;
    }

    if (pos == foff)
        return;

    buf = &bufs[gsl::narrow_cast<size_t>(curr)];
    read = std::ssize(*buf);

    if (read == buf->bsize)
        return;

    R_ASSERT(read == wnd[0]);

    if (pos > foff)
        reopen();

    if (pos == foff)
        return;

    skip(foff - pos);
    pos = foff;
}

void ar_stream::r(void* buffer, gsl::index buffer_size)
{
    auto out = std::span{static_cast<std::byte*>(buffer), gsl::narrow_cast<size_t>(std::min(buffer_size, elapsed()))};

    while (!out.empty())
    {
        if (foff != last)
            next();

        auto& buf = bufs[gsl::narrow_cast<size_t>(curr)];
        auto read = std::ssize(buf);

        if (read == buf.bsize)
            goto copy;

        if (boff == 0 && read == wnd[0])
        {
            const auto direct = xr::rounddown(std::ssize(out), wnd[5]);
            if (direct > 0)
            {
                R_ASSERT(ar::archive_read_data(rd().get(), &out[0], gsl::narrow_cast<size_t>(direct)) == direct);
                pos += direct;

                out = out.subspan(gsl::narrow_cast<size_t>(direct));
                last = foff;
                advance(direct);

                if (out.empty())
                    return;

                continue;
            }
        }

        if (const auto end = boff + std::ssize(out); end > read)
        {
            gsl::index precache;

            if (read == wnd[0] && fsize <= wnd[5])
                precache = fsize;
            else if (read == wnd[0] && end <= wnd[1])
                precache = wnd[1];
            else if (read <= wnd[1] && end <= wnd[2])
                precache = wnd[2] - read;
            else if (read <= wnd[2] && end <= wnd[3])
                precache = wnd[3] - read;
            else if (read <= wnd[3] && end <= wnd[4])
                precache = wnd[4] - read;
            else
                precache = wnd[5] - read;

            precache = std::min(precache, buf.bsize - read);
            buf.resize(gsl::narrow_cast<size_t>(read + precache));
            precache = std::min(precache, fsize - foff - read);

            R_ASSERT(ar::archive_read_data(rd().get(), &buf[gsl::narrow_cast<size_t>(read)], gsl::narrow_cast<size_t>(precache)) == precache);
            pos += precache;
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

CStreamReader* ar_stream::open_chunk(u32 chunk_id)
{
    ::BOOL compressed;

    const auto size = find_chunk(chunk_id, &compressed);
    if (size == 0)
        return nullptr;

    R_ASSERT(!compressed, "cannot stream compressed chunks");

    return xr_new<xr::ar_stream>(ar, cd_pos, tell(), size);
}

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wold-style-cast");

[[nodiscard]] constexpr auto regular(auto type) { return type == AE_IFREG; }

XR_DIAG_POP();

static_assert(sizeof(std::unique_ptr<xr::ar_pool>) == sizeof(uintptr_t));

[[nodiscard]] constexpr auto& ar_cb(uintptr_t& cb) { return *reinterpret_cast<std::unique_ptr<xr::ar_pool>*>(&cb); }
[[nodiscard]] constexpr const auto& ar_cb(const uintptr_t& cb) { return *reinterpret_cast<const std::unique_ptr<xr::ar_pool>*>(&cb); }
} // namespace
} // namespace xr

// [lib]archive

void CLocatorAPI::archive::open_ar() { xr::ar_cb(cb) = std::make_unique<xr::ar_pool>(path.c_str()); }

void CLocatorAPI::archive::autoload_ar()
{
    // [lib]archive doesn't expose archives' user comments, always autoload archives
}

void CLocatorAPI::archive::index_ar(CLocatorAPI& loc, gsl::czstring fs_entry_point) const
{
    auto obj = xr::ar_cb(cb)->acquire_scoped();
    auto& rd = obj.value;

    R_ASSERT(rd.pos == -1);

    for (++rd.pos;; ++rd.pos)
    {
        ar::archive_entry* entry;

        if (const auto ret = ar::archive_read_next_header(rd.get(), &entry); ret != ARCHIVE_OK)
        {
            R_ASSERT(ret == ARCHIVE_EOF);
            return;
        }

        if (!xr::regular(ar::archive_entry_filetype(entry)))
            continue;

        loc.Register(fs_entry_point, ar::archive_entry_pathname_utf8(entry), vfs_idx, gsl::narrow<u64>(rd.pos), ar::archive_entry_size(entry),
                     ar::archive_entry_mtime(entry));
    }
}

IReader* CLocatorAPI::archive::read_ar(const struct file& desc, u32) const
{
    auto obj = xr::ar_cb(cb)->acquire_scoped();
    auto& rd = obj.value;

    rd.goto_entry(gsl::narrow_cast<s32>(desc.cb));

    auto dest = xr_alloc<std::byte>(desc.size_real);
    R_ASSERT(ar::archive_read_data(rd.get(), dest, gsl::narrow_cast<size_t>(desc.size_real)) == desc.size_real);

    return xr_new<CTempReader>(dest, desc.size_real, 0z);
}

CStreamReader* CLocatorAPI::archive::stream_ar(const struct file& desc) const
{
    return xr_new<xr::ar_stream>(*xr::ar_cb(cb), gsl::narrow_cast<s64>(desc.cb), 0, desc.size_real);
}

void CLocatorAPI::archive::close_ar() { xr::ar_cb(cb).reset(); }
