#include "stdafx.h"

#include "LocatorAPI.h"

#include "FS_internal.h"
#include "stream_reader.h"

#include <CallMe.h>
#include <bitmap_object_pool.hpp>

namespace mz
{
#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>
} // namespace mz

namespace xr
{
namespace detail
{
class mz_zip_reader;
}
} // namespace xr

template <>
struct std::default_delete<xr::detail::mz_zip_reader>
{
    constexpr void operator()(xr::detail::mz_zip_reader* ptr) const noexcept
    {
        auto handle = static_cast<void*>(ptr);
        mz::mz_zip_reader_delete(&handle);
    }
};

namespace xr
{
namespace
{
class zip final
{
public:
    class reader final : public std::unique_ptr<xr::detail::mz_zip_reader>
    {
    private:
        [[nodiscard]] constexpr auto& file_info() const { return *reinterpret_cast<mz::mz_zip_file**>(reinterpret_cast<std::byte*>(get()) + 56); }

        [[nodiscard]] static s32 password(void*, void*, mz::mz_zip_file*, gsl::zstring password, s32 max_password);
        [[nodiscard]] static s32 index_one(void* handle, void* userdata, mz::mz_zip_file* file_info);

    public:
        reader() = delete;
        explicit reader(gsl::czstring path);

        void index(CallMe::Delegate<void(gsl::czstring, s64, s64, s64)> reg) const;
        void goto_entry(s64 cd_pos) const;
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
    zip() = delete;
    explicit zip(gsl::czstring path) : path{path} {}

    [[nodiscard]] const auto acquire_scoped() { return pool.acquire_scoped(path); }
};

zip::reader::reader(gsl::czstring path)
{
    reset(static_cast<xr::detail::mz_zip_reader*>(mz::mz_zip_reader_create()));
    mz::mz_zip_reader_set_password_cb(get(), this, &xr::zip::reader::password);
    R_ASSERT(mz::mz_zip_reader_open_file(get(), path) == MZ_OK);

    mz::mz_zip_file* info;
    R_ASSERT(mz::mz_zip_reader_entry_get_info(get(), &info) == MZ_OK);
    R_ASSERT(file_info() == info);
}

s32 zip::reader::password(void*, void*, mz::mz_zip_file*, gsl::zstring password, s32 max_password)
{
    xr_strcpy(password, gsl::narrow_cast<size_t>(max_password), XR_ZIP_PASSWORD);
    return MZ_OK;
}

s32 zip::reader::index_one(void* handle, void* userdata, mz::mz_zip_file* file_info)
{
    if (mz::mz_zip_entry_is_dir(handle) == MZ_OK)
        return 1;

    auto& reg = *static_cast<CallMe::Delegate<void(gsl::czstring, s64, s64, s64)>*>(userdata);
    reg(file_info->filename, mz::mz_zip_get_entry(handle), file_info->uncompressed_size, file_info->modified_date);

    return 1;
}

void zip::reader::index(CallMe::Delegate<void(gsl::czstring, s64, s64, s64)> reg) const
{
    void* handle;

    R_ASSERT(mz::mz_zip_reader_is_open(get()) == MZ_OK);
    R_ASSERT(mz::mz_zip_reader_get_zip_handle(get(), &handle) == MZ_OK);

    if (mz::mz_zip_entry_is_open(handle) == MZ_OK)
        mz::mz_zip_reader_entry_close(get());

    R_ASSERT(mz::mz_zip_locate_first_entry(handle, static_cast<void*>(&reg), &xr::zip::reader::index_one) == MZ_END_OF_LIST);
    file_info() = nullptr;
}

void zip::reader::goto_entry(s64 cd_pos) const
{
    void* handle;

    R_ASSERT(mz::mz_zip_reader_is_open(get()) == MZ_OK);
    R_ASSERT(mz::mz_zip_reader_get_zip_handle(get(), &handle) == MZ_OK);

    if (mz::mz_zip_entry_is_open(handle) == MZ_OK)
        mz::mz_zip_reader_entry_close(get());

    R_ASSERT(mz::mz_zip_goto_entry(handle, cd_pos) == MZ_OK);
    R_ASSERT(mz::mz_zip_entry_get_info(handle, &file_info()) == MZ_OK);
}

// Stream reader for a file inside ZIP

class zip_stream final : public CStreamReader
{
    RTTI_DECLARE_TYPEINFO(xr::zip_stream, CStreamReader);

private:
    class buf final : public xr_vector<std::byte>
    {
    public:
        gsl::index foff;
        gsl::index bsize;
    };

    static constexpr std::array<gsl::index, 6> wnd{0, 1024, 4096, 16384, 65536, 262144};

    decltype(std::declval<xr::zip>().acquire_scoped()) obj;

    std::array<buf, 4> bufs;

    const gsl::index fbase;
    const gsl::index fsize;

    gsl::index foff{};
    gsl::index last{};

    gsl::index curr{};
    gsl::index boff{};

    gsl::index pos;

    xr::zip& zip;
    const s64 cd_pos;

    [[nodiscard]] const auto& rd() const { return obj.value; }

    void reopen();
    void skip(gsl::index len);
    void next();

public:
    zip_stream() = delete;
    explicit zip_stream(xr::zip& zip, s64 cd_pos, gsl::index base, gsl::index sz);
    ~zip_stream() override;

    void close() override;

    [[nodiscard]] gsl::index elapsed() const override { return fsize - tell(); }
    [[nodiscard]] gsl::index length() const override { return fsize; }
    [[nodiscard]] gsl::index tell() const override { return foff + boff; }
    void seek(gsl::index offset) override { advance(offset - tell()); }

    void advance(gsl::index offset) override;
    void r(void* buffer, gsl::index buffer_size) override;
    [[nodiscard]] CStreamReader* open_chunk(u32 chunk_id) override;
};

zip_stream::zip_stream(xr::zip& zip, s64 cd_pos, gsl::index base, gsl::index sz) : obj{zip.acquire_scoped()}, fbase{base}, fsize{sz}, zip{zip}, cd_pos{cd_pos}
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

zip_stream::~zip_stream() { R_ASSERT(mz::mz_zip_reader_entry_close(rd().get()) == MZ_OK); }

void zip_stream::reopen()
{
    rd().goto_entry(cd_pos);
    R_ASSERT(mz::mz_zip_reader_entry_open(rd().get()) == MZ_OK);

    if (fbase > 0)
        skip(fbase);

    pos = 0;
}

void zip_stream::close()
{
    auto self = this;
    xr_delete(self);
}

void zip_stream::advance(gsl::index offset)
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

void zip_stream::skip(gsl::index len)
{
    auto& buf = bufs[gsl::narrow_cast<size_t>(curr)];
    const auto bsize = std::min(buf.bsize, len);

    buf.resize(gsl::narrow_cast<size_t>(bsize));

    for (auto left = len; left > 0;)
    {
        const auto skip = std::min(bsize, left);

        R_ASSERT(mz::mz_zip_reader_entry_read(rd().get(), &buf[0], gsl::narrow_cast<s32>(skip)) == skip);
        left -= skip;
    }

    buf.resize(wnd[0]);
}

void zip_stream::next()
{
    auto buf = &bufs[gsl::narrow_cast<size_t>(curr)];
    auto read = std::ssize(*buf);

    if (read == wnd[0])
        goto pick;

    if (const auto left = std::min(buf->bsize - read, fsize - pos); left > 0)
    {
        buf->resize(gsl::narrow_cast<size_t>(buf->bsize));

        R_ASSERT(mz::mz_zip_reader_entry_read(rd().get(), &(*buf)[gsl::narrow_cast<size_t>(read)], gsl::narrow_cast<s32>(left)) == left);
        pos += left;
    }

pick:
    if (foff > 0)
    {
        if (const auto it = std::ranges::find(bufs, foff, &xr::zip_stream::buf::foff); it == bufs.end())
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

void zip_stream::r(void* buffer, gsl::index buffer_size)
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
                R_ASSERT(mz::mz_zip_reader_entry_read(rd().get(), &out[0], gsl::narrow_cast<s32>(direct)) == direct);
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

            R_ASSERT(mz::mz_zip_reader_entry_read(rd().get(), &buf[gsl::narrow_cast<size_t>(read)], gsl::narrow_cast<s32>(precache)) == precache);
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

CStreamReader* zip_stream::open_chunk(u32 chunk_id)
{
    ::BOOL compressed;

    const auto size = find_chunk(chunk_id, &compressed);
    if (size == 0)
        return nullptr;

    R_ASSERT(!compressed, "cannot stream compressed chunks");

    return xr_new<xr::zip_stream>(zip, cd_pos, tell(), size);
}

static_assert(sizeof(std::unique_ptr<xr::zip>) == sizeof(uintptr_t));

[[nodiscard]] constexpr auto& zip_cb(uintptr_t& cb) { return *reinterpret_cast<std::unique_ptr<xr::zip>*>(&cb); }
[[nodiscard]] constexpr const auto& zip_cb(const uintptr_t& cb) { return *reinterpret_cast<const std::unique_ptr<xr::zip>*>(&cb); }
} // namespace
} // namespace xr

// ZIP

void CLocatorAPI::archive::open_zip() { xr::zip_cb(cb) = std::make_unique<xr::zip>(path.c_str()); }

void CLocatorAPI::archive::autoload_zip()
{
    auto obj = xr::zip_cb(cb)->acquire_scoped();
    gsl::czstring comment;

    if (mz::mz_zip_reader_get_comment(obj.value.get(), &comment) != MZ_OK)
        return;

    IReader rd{comment, xr_strlen(comment) + 1};
    header = std::make_unique<CInifile>(&rd, path.c_str());
}

void CLocatorAPI::archive::index_zip(CLocatorAPI& loc, gsl::czstring fs_entry_point) const
{
    const auto reg = [this, &loc, fs_entry_point](gsl::czstring name, s64 cd_pos, s64 size, s64 modif) {
        loc.Register(fs_entry_point, name, vfs_idx, std::bit_cast<u64>(cd_pos), size, modif);
    };

    xr::zip_cb(cb)->acquire_scoped().value.index(CallMe::fromFunctor(&reg));
}

IReader* CLocatorAPI::archive::read_zip(const struct file& desc, u32) const
{
    auto obj = xr::zip_cb(cb)->acquire_scoped();
    const auto& rd = obj.value;

    rd.goto_entry(std::bit_cast<s64>(desc.cb));
    R_ASSERT(mz::mz_zip_reader_entry_open(rd.get()) == MZ_OK);
    const auto close = gsl::finally([&rd] { R_ASSERT(mz::mz_zip_reader_entry_close(rd.get()) == MZ_OK); });

    auto dest = xr_alloc<std::byte>(desc.size_real);
    R_ASSERT(mz::mz_zip_reader_entry_read(rd.get(), dest, gsl::narrow_cast<s32>(desc.size_real)) == desc.size_real);

    return xr_new<CTempReader>(dest, desc.size_real, 0z);
}

CStreamReader* CLocatorAPI::archive::stream_zip(const struct file& desc) const
{
    return xr_new<xr::zip_stream>(*xr::zip_cb(cb), std::bit_cast<s64>(desc.cb), 0, desc.size_real);
}

void CLocatorAPI::archive::close_zip() { xr::zip_cb(cb).reset(); }
