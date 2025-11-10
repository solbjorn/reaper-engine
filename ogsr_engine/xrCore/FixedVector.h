#ifndef FixedVectorH
#define FixedVectorH

template <typename T, gsl::index dim>
class svector
{
private:
    using adapter_t = std::span<T>;
    using const_adapter_t = std::span<const T>;

public:
    using size_type = std::conditional_t<alignof(T) >= alignof(gsl::index), gsl::index, s32>;
    using value_type = adapter_t::value_type;
    using pointer = adapter_t::pointer;
    using const_pointer = adapter_t::const_pointer;
    using reference = adapter_t::reference;
    using const_reference = adapter_t::const_reference;
    using iterator = adapter_t::iterator;
#ifdef _LIBCPP_ABI_BOUNDED_ITERATORS
    using const_iterator = std::__bounded_iter<const_pointer>;
#else
    using const_iterator = std::__wrap_iter<const_pointer>;
#endif

private:
    std::array<value_type, dim> array{};
    size_type count{};

    [[nodiscard]] constexpr adapter_t adapter() { return adapter_t{data(), gsl::narrow_cast<typename adapter_t::size_type>(count)}; }
    [[nodiscard]] constexpr const_adapter_t adapter() const { return const_adapter_t{data(), gsl::narrow_cast<typename const_adapter_t::size_type>(count)}; }

public:
    constexpr svector() = default;
    constexpr svector(const value_type* p, size_type c) { assign(p, c); }

    [[nodiscard]] constexpr iterator begin() { return adapter().begin(); }
    [[nodiscard]] constexpr iterator end() { return adapter().end(); }
    [[nodiscard]] constexpr const_iterator begin() const { return adapter().begin(); }
    [[nodiscard]] constexpr const_iterator end() const { return adapter().end(); }
    [[nodiscard]] constexpr const_iterator cbegin() const { return adapter().begin(); }
    [[nodiscard]] constexpr const_iterator cend() const { return adapter().end(); }
    [[nodiscard]] constexpr size_type size() const { return count; }
    constexpr void clear() { count = 0; }

    constexpr void resize(size_type c)
    {
        VERIFY(c <= dim);
        count = c;
    }

    constexpr void resize(size_type c, const value_type& fill)
    {
        size_type old = count;
        resize(c);

        if (old >= count)
            return;

        for (size_type i{old}; i < count; ++i)
            array[i] = fill;
    }

    constexpr void reserve(size_type) {}

    constexpr void push_back(const value_type& e)
    {
        VERIFY(count < dim);
        array[count++] = e;
    }

    constexpr void push_back(value_type&& e)
    {
        VERIFY(count < dim);
        array[count++] = std::move(e);
    }

    template <typename... Args>
    constexpr value_type& emplace_back(Args&&... args)
    {
        VERIFY(count < dim);
        return *(new (&array[count++]) value_type{std::forward<Args>(args)...});
    }

    [[nodiscard]] constexpr value_type pop_back()
    {
        VERIFY(count);
        return array[--count];
    }

    [[nodiscard]] constexpr reference operator[](size_type id)
    {
        VERIFY(id < count);
        return adapter()[id];
    }

    [[nodiscard]] constexpr const_reference operator[](size_type id) const
    {
        VERIFY(id < count);
        return adapter()[id];
    }

    [[nodiscard]] constexpr value_type* data() { return array.data(); }
    [[nodiscard]] constexpr const value_type* data() const { return array.data(); }

    [[nodiscard]] constexpr reference front() { return adapter().front(); }
    [[nodiscard]] constexpr reference back() { return adapter().back(); }

    [[nodiscard]] constexpr reference last()
    {
        VERIFY(count < dim);
        return array[count];
    }

    [[nodiscard]] constexpr const_reference front() const { return adapter().front(); }
    [[nodiscard]] constexpr const_reference back() const { return adapter().back(); }

    [[nodiscard]] constexpr const_reference last() const
    {
        VERIFY(count < dim);
        return array[count];
    }

    constexpr void inc() { count++; }
    [[nodiscard]] constexpr bool empty() const { return count == 0; }

    constexpr void erase(size_type id)
    {
        VERIFY(id < count);
        count--;

        for (size_type i{id}; i < count; ++i)
            array[i] = array[i + 1];
    }

    constexpr void erase(iterator it) { erase(it - begin()); }

    constexpr void insert(size_type id, const value_type& V)
    {
        VERIFY(id < count);

        for (size_type i{count}; i > id; --i)
            array[i] = array[i - 1];

        count++;
        array[id] = V;
    }

    constexpr void insert(size_type id, value_type&& V)
    {
        VERIFY(id < count);

        for (size_type i{count}; i > id; --i)
            array[i] = array[i - 1];

        count++;
        array[id] = std::move(V);
    }

    constexpr void assign(const value_type* p, size_type c)
    {
        R_ASSERT(c > 0 && c <= dim);

        std::memcpy(array.data(), p, gsl::narrow_cast<size_t>(c * size_type{sizeof(value_type)}));
        count = c;
    }

    [[nodiscard]] constexpr bool equal(const svector<value_type, dim>& base) const
    {
        if (size() != base.size())
            return false;

        for (const auto [a, b] : std::views::zip(adapter(), base.adapter()))
        {
            if (a != b)
                return false;
        }

        return true;
    }
};

#endif
