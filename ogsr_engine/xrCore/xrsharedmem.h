#ifndef xrsharedmemH
#define xrsharedmemH

//////////////////////////////////////////////////////////////////////////

struct alignas(2 * sizeof(gsl::index)) smem_value
{
    std::atomic<gsl::index> dwReference;
    gsl::index dwSize; // size in bytes !!!

    u64 hash;
    u64 pad;

    std::byte value[];
};
static_assert(offsetof(smem_value, value) == sizeof(smem_value));

//////////////////////////////////////////////////////////////////////////

class smem_container
{
public:
    [[nodiscard]] static smem_value* dock(gsl::index dwSize, const void* ptr);
    static void clean();
    [[nodiscard]] static gsl::index stat_economy();

    static void dump();
};

//////////////////////////////////////////////////////////////////////////

template <typename T>
class ref_smem
{
private:
    smem_value* p_{};

protected:
    // ref-counting
    constexpr void _dec()
    {
        if (p_ == nullptr)
            return;

        if (--p_->dwReference == 0)
            p_ = nullptr;
    }

public:
    constexpr ref_smem<T>& create(gsl::index dwLength, const T* ptr)
    {
        smem_value* v{};

        if (ptr != nullptr && dwLength > 0)
            v = smem_container::dock(dwLength * gsl::index{sizeof(T)}, ptr);
        if (v != nullptr)
            ++v->dwReference;

        _dec();
        p_ = v;

        return *this;
    }

    constexpr ref_smem<T>& _set(const ref_smem<T>& that)
    {
        smem_value* v = that.p_;
        if (v != nullptr)
            ++v->dwReference;

        _dec();
        p_ = v;

        return *this;
    }

    constexpr ref_smem<T>& _set(ref_smem<T>&& that)
    {
        _dec();

        p_ = std::move(that.p_);
        that.p_ = nullptr;

        return *this;
    }

    [[nodiscard]] constexpr const smem_value* _get() const { return p_; }

    // construction
    constexpr ref_smem() = default;
    constexpr ~ref_smem() { _dec(); }

    constexpr ref_smem(const ref_smem<T>& that) { _set(that); }
    constexpr ref_smem(ref_smem<T>&& that) { _set(std::move(that)); }

    // assignment & accessors
    constexpr ref_smem<T>& operator=(const ref_smem<T>& that) { return _set(that); }
    constexpr ref_smem<T>& operator=(ref_smem<T>&& that) { return _set(std::move(that)); }

    [[nodiscard]] constexpr bool operator==(const ref_smem<T>& that) const
    {
        const auto p1 = _get();
        const auto p2 = that._get();

        return p1 == p2 || (p1 != nullptr && p2 != nullptr && p1->hash == p2->hash);
    }

    [[nodiscard]] constexpr auto operator<=>(const ref_smem<T>& that) const
    {
        using ret_t = std::compare_three_way_result_t<std::byte>;

        const auto p1 = _get();
        const auto p2 = that._get();

        if (p1 == p2)
            return ret_t::equal;
        if (p1 == nullptr)
            return ret_t::less;
        if (p2 == nullptr)
            return ret_t::greater;

        if (p1->hash == p2->hash)
            return ret_t::equal;

        const auto mem = std::memcmp(p1->value, p2->value, gsl::narrow_cast<size_t>(std::min(p1->dwSize, p2->dwSize)));
        if (mem != 0)
            return mem < 0 ? ret_t::less : ret_t::greater;

        return p1->dwSize <=> p2->dwSize;
    }

    [[nodiscard]] constexpr explicit operator bool() const { return p_ != nullptr; }
    [[nodiscard]] constexpr T* operator*() { return p_ ? reinterpret_cast<T*>(p_->value) : nullptr; }
    [[nodiscard]] constexpr const T* operator*() const { return p_ ? reinterpret_cast<const T*>(p_->value) : nullptr; }

    [[nodiscard]] constexpr operator std::span<T>() { return p_ ? std::span<T>{**this, gsl::narrow_cast<size_t>(size())} : std::span<T>{}; }
    [[nodiscard]] constexpr operator std::span<const T>() const { return p_ ? std::span<const T>{**this, gsl::narrow_cast<size_t>(size())} : std::span<const T>{}; }

    [[nodiscard]] constexpr T& operator[](gsl::index id) { return std::span<T>{*this}[gsl::narrow_cast<size_t>(id)]; }
    [[nodiscard]] constexpr const T& operator[](gsl::index id) const { return std::span<const T>{*this}[gsl::narrow_cast<size_t>(id)]; }

    // misc func
    [[nodiscard]] constexpr gsl::index size() const { return p_ ? (p_->dwSize / gsl::index{sizeof(T)}) : 0; }
    [[nodiscard]] constexpr bool empty() const { return size() == 0; }

    constexpr void swap(ref_smem<T>& that) { std::swap(p_, that.p_); }

    [[nodiscard]] constexpr bool equal(const ref_smem<T>& that) const { return *this == that; }

    template <typename H>
    [[nodiscard]] friend constexpr H AbslHashValue(H h, const ref_smem<T>& mem)
    {
        return H::combine(std::move(h), std::span<const T>{mem});
    }
};

// externally visible standart functionality
template <typename T>
constexpr void swap(ref_smem<T>& lhs, ref_smem<T>& rhs)
{
    lhs.swap(rhs);
}

#endif
