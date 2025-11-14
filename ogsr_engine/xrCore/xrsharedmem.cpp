#include "stdafx.h"

#include "xrsharedmem.h"

#include <bitmap_object_pool.hpp>

namespace xxh
{
#include <xxhash.h>
}

template <>
struct std::default_delete<smem_value>
{
    constexpr void operator()(smem_value* ptr) const noexcept { xr_free(ptr); }
};

namespace xr
{
namespace
{
class smem_container_impl
{
private:
    using val_t = std::unique_ptr<smem_value>;
    using map_t = xr_unordered_map<xxh::XXH64_hash_t, val_t>;

    tzcnt_utils::BitmapObjectPool<map_t> pool;

public:
    smem_container_impl() { pool.acquire_scoped(); }

    [[nodiscard]] smem_value* insert(gsl::index dwSize, const void* ptr);
    void clean();
    [[nodiscard]] gsl::index stat_economy();

    void dump(std::FILE& f);
};

smem_value* smem_container_impl::insert(gsl::index dwSize, const void* ptr)
{
    const auto size = gsl::narrow_cast<size_t>(dwSize);
    const auto xxh = xxh::XXH3_64bits(ptr, size);

    const auto obj = pool.acquire_scoped();
    auto& map = obj.value;

    const auto iter = map.find(xxh);
    if (iter != map.cend())
        return iter->second.get();

    auto elem = val_t{static_cast<smem_value*>(xr_malloc(gsl::index{sizeof(smem_value)} + dwSize))};
    elem->dwReference = 0;
    elem->dwSize = dwSize;
    elem->hash = xxh;
    std::memcpy(elem->value, ptr, size);

    return map.emplace(xxh, std::move(elem)).first->second.get();
}

void smem_container_impl::clean()
{
    pool.for_each_available([](auto& map) {
        if (absl::erase_if(map, [](const auto& pair) { return pair.second->dwReference == 0; }) > 0)
            map.rehash(0);
    });
}

gsl::index smem_container_impl::stat_economy()
{
    gsl::index ec{-gsl::index{sizeof(smem_container_impl)}};

    pool.for_each_available([&ec](const auto& map) {
        ec -= gsl::index{sizeof(map)};

        for (auto& [_, value] : map)
        {
            ec -= gsl::index{sizeof(map_t::value_type) + sizeof(smem_value)};
            ec += (value->dwReference - 1) * value->dwSize;
        }
    });

    return ec;
}

void smem_container_impl::dump(std::FILE& f)
{
    pool.for_each_available([&f](const auto& map) {
        for (auto& [_, value] : map)
            std::fprintf(&f, "%zd : hash[0x%016llx], %zd bytes\n", value->dwReference.load(), value->hash, value->dwSize);
    });
}

smem_container_impl impl;
} // namespace
} // namespace xr

smem_value* smem_container::dock(gsl::index dwSize, const void* ptr) { return xr::impl.insert(dwSize, ptr); }
void smem_container::clean() { xr::impl.clean(); }
gsl::index smem_container::stat_economy() { return xr::impl.stat_economy(); }

void smem_container::dump()
{
    string_path path;
    std::FILE* f{};

    std::ignore = FS.update_path(path, "$logs$", "$smem_dump$.txt");
    R_ASSERT(fopen_s(&f, path, "w") == 0);
    const auto _ = gsl::finally([f] { std::fclose(f); });

    xr::impl.dump(*f);
}
