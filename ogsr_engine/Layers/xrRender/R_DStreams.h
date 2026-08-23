#ifndef r_DStreamsH
#define r_DStreamsH

#include <wrl/client.h>

namespace xr
{
namespace detail
{
template <typename T, std::size_t A>
struct aligned_allocator : std::allocator<T>
{
private:
    static_assert(A > 0 && std::has_single_bit(A));

    static constexpr auto actual = std::max(A, std::size_t{xr::align_v<T>});

public:
    template <typename U>
    struct rebind
    {
        using other = aligned_allocator<U, A>;
    };

    constexpr aligned_allocator() noexcept = default;

    template <typename U>
    constexpr explicit aligned_allocator(const aligned_allocator<U, A>&) noexcept
    {}

    [[nodiscard]] constexpr T* allocate(std::size_t n) noexcept
    {
        if (n == 0)
            return nullptr;

        return static_cast<T*>(xrMemory::mem_alloc_aligned(gsl::narrow_cast<gsl::index>(xr::roundup(n * sizeof(T), actual)), gsl::index{actual}));
    }

    [[nodiscard]] constexpr std::allocation_result<T*> allocate_at_least(std::size_t n) noexcept
    {
        if (n == 0)
            return {nullptr, 0};

        n = xr::roundup(n * sizeof(T), actual) / sizeof(T);

        return {allocate(n), n};
    }

    constexpr void deallocate(T* p, std::size_t) noexcept
    {
        if (p == nullptr)
            return;

        xrMemory::mem_free_aligned(p);
    }

    [[nodiscard]] constexpr bool operator==(const aligned_allocator&) const noexcept = default;
};
} // namespace detail
} // namespace xr

class _VertexStream final
{
private:
    Microsoft::WRL::ComPtr<ID3DVertexBuffer> pVB;
    ctx_id_t context_id;
    std::size_t mPosition;
    std::vector<std::byte, xr::detail::aligned_allocator<std::byte, 64>> cache;

    constexpr void _clear()
    {
        pVB.Reset();
        context_id = R__INVALID_CTX_ID;
        mPosition = 0;
        cache.clear();
    }

public:
    constexpr _VertexStream() { _clear(); }
    ~_VertexStream() { Destroy(); }

    _VertexStream(const _VertexStream&) = delete;
    _VertexStream& operator=(const _VertexStream&) = delete;

    void Create(ctx_id_t context_id);
    void Destroy();

    [[nodiscard]] constexpr auto Buffer() const { return pVB.Get(); }
    [[nodiscard]] constexpr auto GetSize() const { return cache.size(); }
    constexpr void Flush() { mPosition = cache.size(); }

private:
    [[nodiscard]] void* Lock(std::size_t count, std::size_t stride);
    [[nodiscard]] std::size_t Unlock(std::size_t count, std::size_t stride);

public:
    template <typename T>
    [[nodiscard]] std::span<T> Lock(std::size_t count)
    {
        return std::span{static_cast<T*>(std::assume_aligned<64>(Lock(count, sizeof(T)))), count};
    }

    template <typename T>
    [[nodiscard]] std::size_t Unlock(std::size_t count)
    {
        // The caller is responsible for not rendering anything when @count is zero,
        // this is just a random sentinel and early return (nothing to write).
        if (count == 0)
            return std::numeric_limits<std::size_t>::max();

        return Unlock(count, sizeof(T));
    }
};

class _IndexStream final
{
private:
    static constexpr auto stride{sizeof(u16)};

    Microsoft::WRL::ComPtr<ID3DIndexBuffer> pIB;
    ctx_id_t context_id;
    std::size_t mPosition;
    std::vector<std::byte, xr::detail::aligned_allocator<std::byte, 64>> cache;

    void _clear()
    {
        pIB.Reset();
        context_id = R__INVALID_CTX_ID;
        mPosition = 0;
        cache.clear();
    }

public:
    constexpr _IndexStream() { _clear(); }
    ~_IndexStream() { Destroy(); }

    _IndexStream(const _IndexStream&) = delete;
    _IndexStream& operator=(const _IndexStream&) = delete;

    void Create(ctx_id_t context_id);
    void Destroy();

    [[nodiscard]] constexpr auto Buffer() const { return pIB.Get(); }
    [[nodiscard]] constexpr auto GetSize() const { return cache.size(); }
    constexpr void Flush() { mPosition = cache.size(); }

private:
    [[nodiscard]] u16* lock_raw(std::size_t count);
    [[nodiscard]] std::size_t unlock_raw(std::size_t count);

public:
    [[nodiscard]] std::span<u16> Lock(std::size_t count) { return std::span{std::assume_aligned<64>(lock_raw(count)), count}; }

    [[nodiscard]] std::size_t Unlock(std::size_t count)
    {
        // Same as in _VertexStream::Unlock<>()
        if (count == 0)
            return std::numeric_limits<std::size_t>::max();

        return unlock_raw(count);
    }
};

#endif
