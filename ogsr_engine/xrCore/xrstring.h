#ifndef xrstringH
#define xrstringH

#include <absl/container/flat_hash_map.h>
#include <absl/hash/hash.h>

//////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4200)
struct str_value
{
    u32 dwReference;
    u32 dwLength;
    u64 dwXXH;
    str_value* next;
    char value[];
};

#pragma warning(default : 4200)

struct str_container_impl;

//////////////////////////////////////////////////////////////////////////
class str_container
{
public:
    str_container();
    ~str_container();

    str_value* dock(pcstr value) const;
    void clean() const;
    void dump() const;
    void verify() const;

    [[nodiscard]] size_t stat_economy() const;

private:
    str_container_impl* impl;
};

extern str_container* g_pStringContainer;

//////////////////////////////////////////////////////////////////////////
class shared_str
{
private:
    str_value* p_;

protected:
    // ref-counting
    void _dec()
    {
        if (0 == p_)
            return;
        p_->dwReference--;
        if (0 == p_->dwReference)
            p_ = 0;
    }

public:
    void _set(const char* rhs)
    {
        str_value* v = g_pStringContainer->dock(rhs);
        if (0 != v)
            v->dwReference++;
        _dec();
        p_ = v;
    }
    void _set(shared_str const& rhs)
    {
        str_value* v = rhs.p_;
        if (0 != v)
            v->dwReference++;
        _dec();
        p_ = v;
    }
    const str_value* _get() const { return p_; }

public:
    // construction
    shared_str() { p_ = 0; }
    shared_str(const char* rhs)
    {
        p_ = 0;
        _set(rhs);
    }
    shared_str(shared_str const& rhs)
    {
        p_ = 0;
        _set(rhs);
    }
    ~shared_str() { _dec(); }

    // assignment & accessors
    shared_str& operator=(const char* rhs)
    {
        _set(rhs);
        return *this;
    }
    shared_str& operator=(shared_str const& rhs)
    {
        _set(rhs);
        return *this;
    }

    bool operator==(shared_str const& rhs) const { return _get() == rhs._get(); }
    bool operator!=(shared_str const& rhs) const { return _get() != rhs._get(); }
    bool operator<(shared_str const& rhs) const
    {
        if (!p_)
            return true;
        else
            return strcmp(p_->value, rhs.c_str()) < 0;
    }
    bool operator>(shared_str const&) const = delete;
    char operator[](size_t id) const { return p_->value[id]; }
    bool operator!() const { return !p_; }
    const char* operator*() const { return p_ ? p_->value : nullptr; }

    // Чтобы можно было легко кастить в std::string_view как и все остальные строки
    operator absl::string_view() const { return absl::string_view(data()); }

    const char* c_str() const { return p_ ? p_->value : nullptr; }

    // Используется в погодном редакторе.
    const char* data() const { return p_ ? p_->value : ""; }

    u32 size() const { return p_ ? p_->dwLength : 0; }

    bool empty() const { return size() == 0; }

    void swap(shared_str& rhs)
    {
        str_value* tmp = p_;
        p_ = rhs.p_;
        rhs.p_ = tmp;
    }
    bool equal(const shared_str& rhs) const { return (p_ == rhs.p_); }

    shared_str& __cdecl sprintf(const char* format, ...)
    {
        string4096 buf;
        va_list p;
        va_start(p, format);
        int vs_sz = _vsnprintf(buf, sizeof(buf) - 1, format, p);
        buf[sizeof(buf) - 1] = 0;
        va_end(p);
        if (vs_sz)
            _set(buf);
        return (shared_str&)*this;
    }

    template <typename H>
    friend H AbslHashValue(H h, const shared_str& rhs)
    {
        return H::combine(std::move(h), (absl::string_view)rhs);
    }
};

// string(char)
using xr_string = std::basic_string<char, std::char_traits<char>, xr_allocator<char>>;

DEFINE_VECTOR(xr_string, SStringVec, SStringVecIt);

// externally visible standart functionality
IC void swap(shared_str& lhs, shared_str& rhs) { lhs.swap(rhs); }

IC u32 xr_strlen(const shared_str& a) { return a.size(); }
IC int xr_strcmp(const shared_str& a, const char* b) { return xr_strcmp(*a, b); }
IC int xr_strcmp(const char* a, const shared_str& b) { return xr_strcmp(a, *b); }
IC int xr_strcmp(const shared_str& a, const shared_str& b)
{
    if (a.equal(b))
        return 0;
    else
        return xr_strcmp(*a, *b);
}

IC void xr_strlwr(xr_string& src)
{
    for (xr_string::iterator it = src.begin(); it != src.end(); it++)
        *it = xr_string::value_type(tolower(*it));
}
IC void xr_strlwr(shared_str& src)
{
    if (*src)
    {
        LPSTR lp = xr_strdup(*src);
        xr_strlwr(lp);
        src = lp;
        xr_free(lp);
    }
}

template <class K, class V, class Hash = absl::DefaultHashContainerHash<absl::string_view>, class Eq = absl::DefaultHashContainerEq<absl::string_view>, class Allocator = xr_allocator<std::pair<const K, V>>>
using string_unordered_map = absl::flat_hash_map<K, V, Hash, Eq, Allocator>;

namespace xr_string_utils
{

template <typename T>
inline bool SplitFilename(T& str)
{
    size_t found = str.find_last_of("/\\");
    if (found != T::npos)
    {
        str.erase(found);
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
// trim from start (in place)
template <typename T>
inline void ltrim(T& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
template <typename T>
inline void rtrim(T& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
template <typename T>
inline void trim(T& s)
{
    rtrim(s);
    ltrim(s);
}
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void strlwr(T& data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
}

} // namespace xr_string_utils

#endif
