#ifndef xr_resourceH
#define xr_resourceH

// resource itself, the base class for all derived resources
struct xr_resource : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(xr_resource);

public:
    xr_resource() = default;
    virtual ~xr_resource() = default;

    xr_resource(xr_resource const& other) { *this = other; }

    xr_resource& operator=(xr_resource const& other)
    {
        ref_count.exchange(other.ref_count);
        return *this;
    }

    std::atomic<u32> ref_count{};
};

struct xr_resource_flagged : public xr_resource
{
    RTTI_DECLARE_TYPEINFO(xr_resource_flagged, xr_resource);

public:
    enum
    {
        RF_REGISTERED = 1 << 0,
    };

    u32 dwFlags{};
    int skinning;
    bool hud_disabled;
};

struct xr_resource_named : public xr_resource_flagged
{
    RTTI_DECLARE_TYPEINFO(xr_resource_named, xr_resource_flagged);

public:
    shared_str cName{};

    const char* set_name(const char* name)
    {
        cName = name;
        return *cName;
    }
};

// resptr_BASE
template <class T>
class resptr_base
{
protected:
    T* p_;

    // ref-counting
    void _inc()
    {
        if (p_)
            p_->ref_count++;
    }

    void _dec()
    {
        if (p_ && !--p_->ref_count)
            xr_delete(p_);
    }

public:
    ICF void _set(T* rhs)
    {
        if (rhs)
            rhs->ref_count++;

        _dec();
        p_ = rhs;
    }

    ICF void _set(resptr_base<T> const& rhs)
    {
        T* prhs = rhs._get();
        _set(prhs);
    }

    ICF T* _get() const { return p_; }
    void _clear() { p_ = nullptr; }
};

// resptr_CORE
template <class T, typename C>
class resptr_core : public C
{
protected:
    typedef resptr_core this_type;
    typedef resptr_core<T, C> self;

public:
    // construction
    resptr_core() { C::p_ = nullptr; }

    resptr_core(T* p, const bool add_ref = true)
    {
        C::p_ = p;
        if (add_ref)
            C::_inc();
    }

    resptr_core(const self& rhs)
    {
        C::p_ = rhs.p_;
        C::_inc();
    }

    ~resptr_core() { C::_dec(); }

    // assignment
    self& operator=(const self& rhs)
    {
        this->_set(rhs);
        return (self&)*this;
    }

    // accessors
    T& operator*() const { return *C::p_; }
    T* operator->() const { return C::p_; }

    // unspecified bool type
    typedef T* (resptr_core::*unspecified_bool_type)() const;
    operator unspecified_bool_type() const { return C::p_ == 0 ? 0 : &resptr_core::_get; }

    bool operator!() const { return !C::p_; }

    // fast swapping
    void swap(self& rhs) { std::swap(this->p_, rhs.p_); }
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr

template <class T, class U, typename D>
inline bool operator==(resptr_core<T, D> const& a, resptr_core<U, D> const& b)
{
    return a._get() == b._get();
}

template <class T, class U, typename D>
inline bool operator!=(resptr_core<T, D> const& a, resptr_core<U, D> const& b)
{
    return a._get() != b._get();
}

template <class T, typename D>
inline bool operator==(resptr_core<T, D> const& a, T* b)
{
    return a._get() == b;
}

template <class T, typename D>
inline bool operator!=(resptr_core<T, D> const& a, T* b)
{
    return a._get() != b;
}

template <class T, typename D>
inline bool operator==(T* a, resptr_core<T, D> const& b)
{
    return a == b._get();
}

template <class T, typename D>
inline bool operator!=(T* a, resptr_core<T, D> const& b)
{
    return a != b._get();
}

template <class T, typename D>
inline bool operator<(resptr_core<T, D> const& a, resptr_core<T, D> const& b)
{
    return std::less<T*>()(a._get(), b._get());
}

template <class T, typename D>
inline bool operator>(resptr_core<T, D> const& a, resptr_core<T, D> const& b)
{
    return std::less<T*>()(b._get(), a._get());
}

// externally visible swap
template <class T, typename D>
void swap(resptr_core<T, D>& lhs, resptr_core<T, D>& rhs)
{
    lhs.swap(rhs);
}

// mem_fn support
template <class T, typename D>
T* get_pointer(resptr_core<T, D> const& p)
{
    return p.get();
}

// casting
template <class T, class U, typename D>
resptr_core<T, D> static_pointer_cast(resptr_core<U, D> const& p)
{
    return static_cast<T*>(p.get());
}

template <class T, class U, typename D>
resptr_core<T, D> dynamic_pointer_cast(resptr_core<U, D> const& p)
{
    return smart_cast<T*>(p.get());
}

#endif // xr_resourceH
