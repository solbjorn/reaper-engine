#include "stdafx.h"

#include "xrstring.h"

#include <bitmap_object_pool.hpp>

namespace xxh
{
#include <xxhash.h>
}

#pragma comment(lib, "xxhash")

template <>
struct std::default_delete<str_value>
{
    constexpr void operator()(str_value* ptr) const noexcept { xr_free(ptr); }
};

namespace xr
{
namespace
{
class str_container_impl
{
private:
    using val_t = std::unique_ptr<str_value>;
    using map_t = xr_unordered_map<xxh::XXH64_hash_t, val_t>;

    static constexpr size_t base_cap{4096};
    static constexpr size_t first_base_cap{2 * base_cap};

    class pool_t final : public tzcnt_utils::BitmapObjectPoolImpl<map_t, pool_t>
    {
        friend class tzcnt_utils::BitmapObjectPoolImpl<map_t, pool_t>;

        void initialize(void* location, size_t cap)
        {
            auto* map = new (location) map_t{};
            map->reserve(cap);
        }
    };

    pool_t pool;

public:
    str_container_impl()
    {
        // Pre-populate 1 map with doubled default capacity
        pool.acquire_scoped(first_base_cap);
    }

    [[nodiscard]] str_value* insert(gsl::czstring str);
    void clean();
    [[nodiscard]] gsl::index stat_economy();

    void verify();
    void dump(std::FILE& f);
};

str_value* str_container_impl::insert(gsl::czstring str)
{
    const std::string_view sv{str};
    const auto xxh = xxh::XXH3_64bits(sv.data(), sv.size());

    const auto obj = pool.acquire_scoped(base_cap);
    auto& map = obj.value;

    const auto iter = map.find(xxh);
    if (iter != map.cend())
        return iter->second.get();

    auto elem = val_t{static_cast<str_value*>(xr_malloc(gsl::index{sizeof(str_value)} + std::ssize(sv) + 1))};
    elem->dwReference = 0;
    elem->dwLength = std::ssize(sv);
    elem->hash = xxh;
    std::memcpy(elem->value, sv.data(), sv.size() + 1);

    return map.emplace(xxh, std::move(elem)).first->second.get();
}

void str_container_impl::clean()
{
    pool.for_each_available([](auto& map) {
        if (absl::erase_if(map, [](const auto& pair) { return pair.second->dwReference == 0; }) > 0)
            map.rehash(0);
    });
}

gsl::index str_container_impl::stat_economy()
{
    gsl::index ec{-gsl::index{sizeof(str_container_impl)}};

    pool.for_each_available([&ec](const auto& map) {
        ec -= gsl::index{sizeof(map)};

        for (auto& [_, value] : map)
        {
            ec -= gsl::index{sizeof(map_t::value_type) + sizeof(str_value)};
            ec += (value->dwReference - 1) * (value->dwLength + 1);
        }
    });

    return ec;
}

void str_container_impl::verify()
{
    pool.for_each_available([](const auto& map) {
        for (auto& [_, value] : map)
        {
            const std::string_view sv{value->value};
            ASSERT_FMT(value->dwLength == std::ssize(sv), "corrupted shared string length: %zd bytes, expected %zd bytes", value->dwLength, std::ssize(sv));

            const auto xxh = xxh::XXH3_64bits(sv.data(), sv.size());
            ASSERT_FMT(value->hash == xxh, "corrupted shared string hash: 0x%016llx, expected 0x%016llx", value->hash, xxh);
        }
    });
}

void str_container_impl::dump(std::FILE& f)
{
    pool.for_each_available([&f](const auto& map) {
        for (auto& [_, value] : map)
            std::fprintf(&f, "ref[%zd]-len[%zd]-hash[0x%016llx]: %s\n", value->dwReference.load(), value->dwLength, value->hash, value->value);
    });
}

str_container_impl impl;
} // namespace
} // namespace xr

str_value* str_container::dock(gsl::czstring value) { return xr::impl.insert(value); }
void str_container::clean() { xr::impl.clean(); }
gsl::index str_container::stat_economy() { return xr::impl.stat_economy(); }

void str_container::verify() { xr::impl.verify(); }

void str_container::dump()
{
    string_path path;
    std::FILE* f{};

    std::ignore = FS.update_path(path, "$logs$", "$str_dump$.txt");
    R_ASSERT(fopen_s(&f, path, "w") == 0);
    const auto _ = gsl::finally([f] { std::fclose(f); });

    xr::impl.dump(*f);
}
