#include "stdafx.h"

#include "xrstring.h"

#include <concurrentqueue.h>

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

    moodycamel::ConcurrentQueue<map_t> q;

    class map_vector : public xr_vector<map_t>
    {
    private:
        decltype(q)& queue;

    public:
        map_vector() = delete;
        map_vector(map_vector&&) = delete;

        explicit map_vector(decltype(queue) in) : queue{in}
        {
            reserve(queue.size_approx());

            while (true)
            {
                map_t map;

                if (!queue.try_dequeue(map))
                    break;

                emplace_back(std::move(map));
            }
        }

        ~map_vector()
        {
            for (auto&& map : *this)
                queue.enqueue(std::move(map));
        }
    };

    static constexpr size_t base_cap{4096};
    static constexpr size_t first_base_cap{2 * base_cap};

    [[nodiscard]] auto get()
    {
        map_t map;

        if (!q.try_dequeue(map))
            map.reserve(base_cap);

        return map;
    }

public:
    str_container_impl()
    {
        map_t map;
        map.reserve(first_base_cap);
        q.enqueue(std::move(map));
    }

    [[nodiscard]] str_value* insert(gsl::czstring str);
    void clean();
    [[nodiscard]] gsl::index stat_economy();

    void verify();
    void dump(FILE& f);
};

str_value* str_container_impl::insert(gsl::czstring str)
{
    if (str == nullptr) [[unlikely]]
        return nullptr;

    const absl::string_view sv{str};
    const auto xxh = xxh::XXH3_64bits(sv.data(), sv.size());

    auto map = get();
    const auto _ = gsl::finally([this, &map] { q.enqueue(std::move(map)); });

    const auto iter = map.find(xxh);
    if (iter != map.cend())
        return iter->second.get();

    auto elem = val_t{static_cast<str_value*>(xr_malloc(sizeof(str_value) + sv.size() + 1))};
    elem->dwReference = 0;
    elem->dwLength = std::ssize(sv);
    elem->hash = xxh;
    std::memcpy(elem->value, sv.data(), sv.size() + 1);

    return map.emplace(xxh, std::move(elem)).first->second.get();
}

void str_container_impl::clean()
{
    for (auto& map : map_vector{q})
    {
        if (absl::erase_if(map, [](const auto& pair) { return pair.second->dwReference == 0; }) > 0)
            map.rehash(0);
    }
}

gsl::index str_container_impl::stat_economy()
{
    gsl::index ec{-gsl::index{sizeof(str_container_impl)}};

    for (const auto& map : map_vector{q})
    {
        ec -= gsl::index{sizeof(map)};

        for (auto& [_, value] : map)
        {
            ec -= gsl::index{sizeof(map_t::value_type) + sizeof(str_value)};
            ec += (value->dwReference - 1) * (value->dwLength + 1);
        }
    }

    return ec;
}

void str_container_impl::verify()
{
    for (const auto& map : map_vector{q})
    {
        for (auto& [_, value] : map)
        {
            const absl::string_view sv{value->value};
            ASSERT_FMT(value->dwLength == std::ssize(sv), "corrupted shared string length: %zd bytes, expected %zd bytes", value->dwLength, std::ssize(sv));

            const auto xxh = xxh::XXH3_64bits(sv.data(), sv.size());
            ASSERT_FMT(value->hash == xxh, "corrupted shared string hash: 0x%016llx, expected 0x%016llx", value->hash, xxh);
        }
    }
}

void str_container_impl::dump(FILE& f)
{
    for (const auto& map : map_vector{q})
    {
        for (auto& [_, value] : map)
            fprintf(&f, "ref[%zd]-len[%zd]-hash[0x%016llx]: %s\n", value->dwReference.load(), value->dwLength, value->hash, value->value);
    }
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
    FILE* f{};

    FS.update_path(path, "$logs$", "$str_dump$.txt");
    R_ASSERT(fopen_s(&f, path, "w") == 0);
    const auto _ = gsl::finally([f] { fclose(f); });

    xr::impl.dump(*f);
}
