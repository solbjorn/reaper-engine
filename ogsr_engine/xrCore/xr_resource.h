#ifndef xr_resourceH
#define xr_resourceH

// resource itself, the base class for all derived resources
struct xr_resource : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(xr_resource);

public:
    std::atomic<gsl::index> ref_count{};

    xr_resource() = default;
    ~xr_resource() override = default;

    void clone(const xr_resource& from) { ref_count = from.ref_count.load(); }
};

struct xr_resource_flagged : public xr_resource
{
    RTTI_DECLARE_TYPEINFO(xr_resource_flagged, xr_resource);

public:
    enum
    {
        RF_REGISTERED = 1 << 0,
        RF_HUD_DISABLED = 1 << 1,
    };

    u32 dwFlags{};
    s32 skinning{};

    ~xr_resource_flagged() override = default;

    void clone(const xr_resource_flagged& from)
    {
        xr_resource::clone(from);

        dwFlags = from.dwFlags;
        skinning = from.skinning;
    }
};

struct xr_resource_named : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(xr_resource_named, xr_resource_flagged);

public:
    shared_str cName{};

    ~xr_resource_named() override = default;

    const char* set_name(const char* name)
    {
        cName._set(name);
        return *cName;
    }
};

// resptr_BASE
template <typename T>
class resptr_base
{
protected:
    T* p_{};

    // ref-counting
    constexpr void _inc()
    {
        if (p_ != nullptr)
            ++p_->ref_count;
    }

    constexpr void _dec()
    {
        if (p_ != nullptr && --p_->ref_count == 0)
            xr_delete(p_);
    }

public:
    constexpr void _set(T* rhs)
    {
        if (rhs != nullptr)
            ++rhs->ref_count;

        _dec();
        p_ = rhs;
    }

    constexpr void _set(const resptr_base<T>& rhs)
    {
        T* prhs = rhs._get();
        _set(prhs);
    }

    [[nodiscard]] constexpr T* _get() const { return p_; }
    constexpr void _clear() { p_ = nullptr; }
};

// resptr_CORE
template <typename T, typename C>
class resptr_core : public C
{
protected:
    typedef resptr_core this_type;
    typedef resptr_core<T, C> self;

public:
    // construction
    constexpr resptr_core() { C::p_ = nullptr; }

    constexpr explicit resptr_core(T* p, bool add_ref = true)
    {
        C::p_ = p;
        if (add_ref)
            C::_inc();
    }

    constexpr resptr_core(const self& rhs)
    {
        C::p_ = rhs.p_;
        C::_inc();
    }

    constexpr ~resptr_core() { C::_dec(); }

    // assignment
    constexpr self& operator=(const self& rhs)
    {
        this->_set(rhs);
        return *this;
    }

    // accessors
    [[nodiscard]] constexpr T& operator*() const { return *C::p_; }
    [[nodiscard]] constexpr T* operator->() const { return C::p_; }

    // unspecified bool type
    typedef T* (resptr_core::*unspecified_bool_type)() const;

    [[nodiscard]] constexpr operator unspecified_bool_type() const { return C::p_ ? &resptr_core::_get : nullptr; }
    [[nodiscard]] constexpr explicit operator bool() const { return C::p_ != nullptr; }

    // fast swapping
    constexpr void swap(self& rhs) { std::swap(this->p_, rhs.p_); }
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr

template <typename T, typename U, typename D>
[[nodiscard]] constexpr bool operator==(const resptr_core<T, D>& a, const resptr_core<U, D>& b)
{
    return a._get() == b._get();
}

template <typename T, typename U, typename D>
[[nodiscard]] constexpr bool operator!=(const resptr_core<T, D>& a, const resptr_core<U, D>& b)
{
    return a._get() != b._get();
}

template <typename T, typename D>
[[nodiscard]] constexpr bool operator==(const resptr_core<T, D>& a, const T* b)
{
    return a._get() == b;
}

template <typename T, typename D>
[[nodiscard]] constexpr bool operator!=(const resptr_core<T, D>& a, const T* b)
{
    return a._get() != b;
}

template <typename T, typename D>
[[nodiscard]] constexpr bool operator==(const T* a, const resptr_core<T, D>& b)
{
    return a == b._get();
}

template <typename T, typename D>
[[nodiscard]] constexpr bool operator!=(const T* a, const resptr_core<T, D>& b)
{
    return a != b._get();
}

template <typename T, typename D>
[[nodiscard]] constexpr bool operator<(const resptr_core<T, D>& a, const resptr_core<T, D>& b)
{
    return std::less<T*>()(a._get(), b._get());
}

template <typename T, typename D>
[[nodiscard]] constexpr bool operator>(const resptr_core<T, D>& a, const resptr_core<T, D>& b)
{
    return std::less<T*>()(b._get(), a._get());
}

// externally visible swap
template <typename T, typename D>
constexpr void swap(resptr_core<T, D>& lhs, resptr_core<T, D>& rhs)
{
    lhs.swap(rhs);
}

// mem_fn support
template <typename T, typename D>
[[nodiscard]] constexpr T* get_pointer(const resptr_core<T, D>& p)
{
    return p.get();
}

// casting
template <typename T, typename U, typename D>
[[nodiscard]] constexpr resptr_core<T, D> static_pointer_cast(const resptr_core<U, D>& p)
{
    return static_cast<T*>(p.get());
}

template <typename T, typename U, typename D>
[[nodiscard]] constexpr resptr_core<T, D> dynamic_pointer_cast(const resptr_core<U, D>& p)
{
    return smart_cast<T*>(p.get());
}

#endif // xr_resourceH
