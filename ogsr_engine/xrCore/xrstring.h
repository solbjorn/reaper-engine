#ifndef xrstringH
#define xrstringH

#include <absl/container/flat_hash_map.h>
#include <absl/hash/hash.h>

//////////////////////////////////////////////////////////////////////////

struct alignas(2 * sizeof(gsl::index)) str_value
{
    std::atomic<gsl::index> dwReference;
    gsl::index dwLength;

    u64 hash;
    u64 pad;

    char value[];
};
static_assert(offsetof(str_value, value) == sizeof(str_value));

//////////////////////////////////////////////////////////////////////////

class str_container
{
public:
    [[nodiscard]] static str_value* dock(gsl::czstring value);
    static void clean();
    [[nodiscard]] static gsl::index stat_economy();

    static void dump();
    static void verify();
};

//////////////////////////////////////////////////////////////////////////

class shared_str
{
private:
    str_value* p_{};

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
    constexpr shared_str& _set(gsl::czstring str)
    {
        str_value* v{};

        if (str != nullptr)
            v = str_container::dock(str);
        if (v != nullptr)
            ++v->dwReference;

        _dec();
        p_ = v;

        return *this;
    }

    constexpr shared_str& _set(const shared_str& that)
    {
        str_value* v = that.p_;
        if (v != nullptr)
            ++v->dwReference;

        _dec();
        p_ = v;

        return *this;
    }

    constexpr shared_str& _set(shared_str&& that)
    {
        _dec();

        p_ = std::move(that.p_);
        that.p_ = nullptr;

        return *this;
    }

    [[nodiscard]] constexpr const str_value* _get() const { return p_; }

    // construction
    constexpr shared_str() = default;
    constexpr explicit shared_str(gsl::czstring that) { _set(that); }
    constexpr ~shared_str() { _dec(); }

    constexpr shared_str(const shared_str& that) { _set(that); }
    constexpr shared_str(shared_str&& that) { _set(std::move(that)); }

    // assignment & accessors
    constexpr shared_str& operator=(const shared_str& that) { return _set(that); }
    constexpr shared_str& operator=(shared_str&& that) { return _set(std::move(that)); }

    [[nodiscard]] constexpr bool operator==(const shared_str& that) const
    {
        const auto p1 = _get();
        const auto p2 = that._get();

        return p1 == p2 || (p1 != nullptr && p2 != nullptr && p1->hash == p2->hash);
    }

    [[nodiscard]] constexpr auto operator<=>(const shared_str& that) const
    {
        using ret_t = std::compare_three_way_result_t<absl::string_view>;

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

        return absl::string_view{*this} < absl::string_view{that} ? ret_t::less : ret_t::greater;
    }

    [[nodiscard]] constexpr explicit operator bool() const { return p_ != nullptr; }
    [[nodiscard]] constexpr gsl::czstring operator*() const { return p_ ? p_->value : nullptr; }

    // Чтобы можно было легко кастить в absl::string_view как и все остальные строки
    [[nodiscard]] constexpr operator absl::string_view() const { return p_ ? absl::string_view{p_->value, gsl::narrow_cast<size_t>(p_->dwLength)} : absl::string_view{}; }

    [[nodiscard]] constexpr gsl::czstring c_str() const { return p_ ? p_->value : nullptr; }
    // Используется в погодном редакторе.
    [[nodiscard]] constexpr gsl::czstring data() const { return p_ ? p_->value : ""; }

    [[nodiscard]] constexpr gsl::index size() const { return p_ ? p_->dwLength : 0; }
    [[nodiscard]] constexpr bool empty() const { return size() == 0; }

    constexpr void swap(shared_str& that) { std::swap(p_, that.p_); }

    [[nodiscard]] constexpr bool equal(const shared_str& that) const { return *this == that; }

    shared_str& XR_PRINTF(2, 3) sprintf(gsl::czstring format, ...)
    {
        std::va_list args, args_copy;

        va_start(args, format);
        va_copy(args_copy, args);

        const auto sz = std::vsnprintf(nullptr, 0, format, args);
        if (sz <= 0)
        {
            p_ = nullptr;
            return *this;
        }

        const auto n = gsl::narrow_cast<size_t>(sz + 1);
        std::string result(n, ' ');
        std::vsnprintf(result.data(), n, format, args_copy);

        va_end(args_copy);
        va_end(args);

        _set(result.c_str());

        return *this;
    }

    template <typename H>
    friend constexpr H AbslHashValue(H h, const shared_str& str)
    {
        return H::combine(std::move(h), absl::string_view{str});
    }
};

// string(char)
using xr_string = std::basic_string<char, std::char_traits<char>, xr_allocator<char>>;

DEFINE_VECTOR(xr_string, SStringVec, SStringVecIt);

// externally visible standart functionality
constexpr void swap(shared_str& lhs, shared_str& rhs) { lhs.swap(rhs); }

[[nodiscard]] constexpr gsl::index xr_strlen(const shared_str& a) { return a.size(); }

[[nodiscard]] constexpr auto xr_strcmp(const shared_str& a, absl::string_view b) { return xr_strcmp(absl::string_view{a}, b); }
[[nodiscard]] constexpr auto xr_strcmp(absl::string_view a, const shared_str& b) { return xr_strcmp(a, absl::string_view{b}); }

[[nodiscard]] constexpr auto xr_strcmp(const shared_str& a, gsl::czstring b) { return xr_strcmp(absl::string_view{a}, absl::string_view{b}); }
[[nodiscard]] constexpr auto xr_strcmp(gsl::czstring a, const shared_str& b) { return xr_strcmp(absl::string_view{a}, absl::string_view{b}); }

[[nodiscard]] constexpr auto xr_strcmp(const shared_str& a, const shared_str& b) { return a <=> b; }

constexpr void xr_strlwr(xr_string& src)
{
    for (auto& c : src)
        c = xr::tolower(c);
}

IC void xr_strlwr(shared_str& src)
{
    if (*src)
    {
        LPSTR lp = xr_strdup(*src);
        xr_strlwr(lp);
        src._set(lp);
        xr_free(lp);
    }
}

template <class K, class V, class Hash = absl::DefaultHashContainerHash<absl::string_view>, class Eq = absl::DefaultHashContainerEq<absl::string_view>,
          class Allocator = xr_allocator<std::pair<const K, V>>>
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
} // namespace xr_string_utils

#endif
