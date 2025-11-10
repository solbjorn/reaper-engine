#include "stdafx.h"

#include "xrsharedmem.h"

#include <concurrentqueue.h>

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

    [[nodiscard]] auto get()
    {
        map_t map;
        q.try_dequeue(map);
        return map;
    }

public:
    smem_container_impl() { q.enqueue({}); }

    [[nodiscard]] smem_value* insert(gsl::index dwSize, const void* ptr);
    void clean();
    [[nodiscard]] gsl::index stat_economy();

    void dump(FILE& f);
};

smem_value* smem_container_impl::insert(gsl::index dwSize, const void* ptr)
{
    const auto size = gsl::narrow_cast<size_t>(dwSize);
    const auto xxh = xxh::XXH3_64bits(ptr, size);

    auto map = get();
    const auto _ = gsl::finally([this, &map] { q.enqueue(std::move(map)); });

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
    for (auto& map : map_vector{q})
    {
        if (absl::erase_if(map, [](const auto& pair) { return pair.second->dwReference == 0; }) > 0)
            map.rehash(0);
    }
}

gsl::index smem_container_impl::stat_economy()
{
    gsl::index ec{-gsl::index{sizeof(smem_container_impl)}};

    for (const auto& map : map_vector{q})
    {
        ec -= gsl::index{sizeof(map)};

        for (auto& [_, value] : map)
        {
            ec -= gsl::index{sizeof(map_t::value_type) + sizeof(smem_value)};
            ec += (value->dwReference - 1) * value->dwSize;
        }
    }

    return ec;
}

void smem_container_impl::dump(FILE& f)
{
    for (const auto& map : map_vector{q})
    {
        for (auto& [_, value] : map)
            fprintf(&f, "%zd : hash[0x%016llx], %zd bytes\n", value->dwReference.load(), value->hash, value->dwSize);
    }
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
    FILE* f{};

    std::ignore = FS.update_path(path, "$logs$", "$smem_dump$.txt");
    R_ASSERT(fopen_s(&f, path, "w") == 0);
    const auto _ = gsl::finally([f] { fclose(f); });

    xr::impl.dump(*f);
}
