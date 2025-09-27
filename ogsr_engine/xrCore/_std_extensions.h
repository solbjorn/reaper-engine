#ifndef __XRCORE_STD_EXTENSIONS_H
#define __XRCORE_STD_EXTENSIONS_H

#include <immintrin.h>

#include <algorithm>
#include <memory>

#ifdef abs
#undef abs
#endif

#ifdef _MIN
#undef _MIN
#endif

#ifdef _MAX
#undef _MAX
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// token type definition
struct alignas(2 * sizeof(gsl::index)) xr_token
{
    LPCSTR name;
    gsl::index id;
};

IC LPCSTR get_token_name(const xr_token* tokens, gsl::index key)
{
    for (gsl::index k = 0; tokens[k].name; k++)
    {
        if (key == tokens[k].id)
            return tokens[k].name;
    }

    return "";
}

// https://www.fluentcpp.com/2017/10/27/function-aliases-cpp/
#define XR_FUNCTION_ALIAS(highLevelF, lowLevelF) \
    template <typename... Args> \
    constexpr ICF auto highLevelF(Args&&... args) -> decltype(lowLevelF(std::forward<Args>(args)...)) \
    { \
        return lowLevelF(std::forward<Args>(args)...); \
    } \
    XR_MACRO_END()

// Allow passing explicit template arguments
#define XR_FUNCTION_ALIAS_1(highLevelF, lowLevelF) \
    XR_FUNCTION_ALIAS(highLevelF, lowLevelF); \
\
    template <typename A0, typename... Args> \
    constexpr ICF auto highLevelF(Args&&... args) -> decltype(lowLevelF<A0>(std::forward<Args>(args)...)) \
    { \
        return lowLevelF<A0>(std::forward<Args>(args)...); \
    } \
    XR_MACRO_END()

XR_FUNCTION_ALIAS(_min, std::min);
XR_FUNCTION_ALIAS(_max, std::max);

template <typename T>
constexpr ICF T _sqr(T a)
{
    return a * a;
}

XR_FUNCTION_ALIAS(_abs, std::abs);
XR_FUNCTION_ALIAS(_sqrt, std::sqrt);
XR_FUNCTION_ALIAS(_sin, std::sin);
XR_FUNCTION_ALIAS(_cos, std::cos);

template <std::floating_point T>
constexpr inline bool _valid(T x)
{
    switch (std::fpclassify(x))
    {
    case FP_NORMAL:
    case FP_ZERO: return true;
    default: return false;
    }
}

char* timestamp(string64& dest);

// Round @x to next or prev @a boundary, where @a is a power of two

template <std::unsigned_integral T>
constexpr ICF T round_mask(T x, T mask)
{
    return (x + mask) & ~mask;
}

template <std::unsigned_integral T>
constexpr ICF T roundup(T x, T a)
{
    return round_mask(x, a - 1);
}

template <std::unsigned_integral T>
constexpr ICF T rounddown(T x, T a)
{
    return roundup(x - (a - 1), a);
}

// Pack 64-bit value into <= 32 bits
// https://elixir.bootlin.com/linux/v6.13-rc3/source/include/linux/hash.h#L24
constexpr ICF u32 hash_64(u64 val, u32 bits)
{
    constexpr u64 GOLDEN_RATIO_64{0x61C8864680B583EBull};

    return gsl::narrow_cast<u32>(val * GOLDEN_RATIO_64 >> (64 - bits));
}

constexpr ICF void xr_memcpy_const(void* dst, const void* src, size_t size)
{
    std::byte* cdst = static_cast<std::byte*>(dst);
    const std::byte* csrc = static_cast<const std::byte*>(src);

    for (size_t i = 0; i < size; i++)
        cdst[i] = csrc[i];
}

constexpr ICF void xr_memcpy16(void* dst, const void* src)
{
    if (std::is_constant_evaluated())
        xr_memcpy_const(dst, src, 16);
    else
        _mm_store_si128(static_cast<__m128i*>(std::assume_aligned<sizeof(__m128i)>(dst)), _mm_load_si128(static_cast<const __m128i*>(std::assume_aligned<sizeof(__m128i)>(src))));
}

constexpr ICF void xr_memcpy128(void* dst, const void* src, size_t size)
{
    if (std::is_constant_evaluated())
        return xr_memcpy_const(dst, src, size);

    __m128i* cdst = static_cast<__m128i*>(std::assume_aligned<sizeof(__m128i)>(dst));
    const __m128i* csrc = static_cast<const __m128i*>(std::assume_aligned<sizeof(__m128i)>(src));

    do
    {
        _mm_store_si128(cdst, _mm_load_si128(csrc));
        cdst++;
        csrc++;
        size -= 16;
    } while (size >= 16);
}

constexpr ICF void xr_memcpy32(void* dst, const void* src)
{
    if (std::is_constant_evaluated())
        xr_memcpy_const(dst, src, 32);
    else
        _mm256_store_si256(static_cast<__m256i*>(std::assume_aligned<sizeof(__m256i)>(dst)),
                           _mm256_load_si256(static_cast<const __m256i*>(std::assume_aligned<sizeof(__m256i)>(src))));
}

constexpr ICF void xr_memcpy256(void* dst, const void* src, size_t size)
{
    if (std::is_constant_evaluated())
        return xr_memcpy_const(dst, src, size);

    __m256i* cdst = static_cast<__m256i*>(std::assume_aligned<sizeof(__m256i)>(dst));
    const __m256i* csrc = static_cast<const __m256i*>(std::assume_aligned<sizeof(__m256i)>(src));

    do
    {
        _mm256_store_si256(cdst, _mm256_load_si256(csrc));
        cdst++;
        csrc++;
        size -= 32;
    } while (size >= 32);
}

constexpr ICF void xr_memcpy(void* dst, const void* src, size_t size)
{
    if (std::is_constant_evaluated())
        return xr_memcpy_const(dst, src, size);

    if (size >= 32 && !((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 31))
    {
        __m256i* cdst = static_cast<__m256i*>(std::assume_aligned<sizeof(__m256i)>(dst));
        const __m256i* csrc = static_cast<const __m256i*>(std::assume_aligned<sizeof(__m256i)>(src));

        do
        {
            _mm256_store_si256(cdst, _mm256_load_si256(csrc));
            cdst++;
            csrc++;
            size -= 32;
        } while (size >= 32);

        dst = static_cast<void*>(cdst);
        src = static_cast<const void*>(csrc);
    }

    if (size >= 16 && !((reinterpret_cast<uintptr_t>(dst) | reinterpret_cast<uintptr_t>(src)) & 15))
    {
        __m128i* cdst = static_cast<__m128i*>(std::assume_aligned<sizeof(__m128i)>(dst));
        const __m128i* csrc = static_cast<const __m128i*>(std::assume_aligned<sizeof(__m128i)>(src));

        do
        {
            _mm_store_si128(cdst, _mm_load_si128(csrc));
            cdst++;
            csrc++;
            size -= 16;
        } while (size >= 16);

        dst = static_cast<void*>(cdst);
        src = static_cast<const void*>(csrc);
    }

    while (size >= 8)
    {
        u64* cdst = static_cast<u64*>(dst);
        const u64* csrc = static_cast<const u64*>(src);

        *cdst = *csrc;
        cdst++;
        csrc++;
        size -= 8;

        dst = static_cast<void*>(cdst);
        src = static_cast<const void*>(csrc);
    }

    if (size >= 4)
    {
        u32* cdst = static_cast<u32*>(dst);
        const u32* csrc = static_cast<const u32*>(src);

        *cdst = *csrc;
        cdst++;
        csrc++;
        size -= 4;

        dst = static_cast<void*>(cdst);
        src = static_cast<const void*>(csrc);
    }

    std::byte* cdst = static_cast<std::byte*>(dst);
    const std::byte* csrc = static_cast<const std::byte*>(src);

    while (size)
    {
        *cdst = *csrc;
        cdst++;
        csrc++;
        size--;
    }
}

// return pointer to ".ext"
IC char* strext(const char* S) { return (char*)strrchr(S, '.'); }

IC u32 xr_strlen(const char* S) { return (u32)strlen(S); }

IC char* xr_strlwr(char* S) { return _strlwr(S); }

IC int xr_strcmp(const char* S1, const char* S2) { return strcmp(S1, S2); }

char* xr_strdup(const char* string);

template <typename StrType, typename StrType2, typename... Args>
inline char* xr_strconcat(StrType& dest, const StrType2& arg1, const Args&... args)
{
    static_assert(std::is_array_v<StrType>);
    static_assert(std::is_same_v<std::remove_extent_t<StrType>, char>);

    strcpy_s(dest, arg1);

    (strcat_s(dest, args), ...);

    return &dest[0];
}

#define strconcat(unused_arg, ...) xr_strconcat(__VA_ARGS__)
#define xr_strcpy strcpy_s
#define xr_sprintf sprintf_s
#define xr_strcat strcat_s

std::string StringToUTF8(const char* in);
std::string StringFromUTF8(const char* in);
// Определяет есть ли в строке юникодные символы
inline bool StringHasUTF8(const char* str)
{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(str);
    while (*p != 0)
    {
        if (*p < 0x80)
        {
            // однобайтовый символ в UTF-8, пропускаем
            p++;
        }
        else if (*p < 0xc2 || *p > 0xf4)
        {
            // байт не может быть первым байтом в UTF-8, строка содержит ошибку
            return false;
        }
        else if (*p < 0xe0)
        {
            // двухбайтовый символ в UTF-8
            if (*(p + 1) < 0x80 || *(p + 1) > 0xbf)
            {
                // неправильный второй байт, строка содержит ошибку
                return false;
            }
            p += 2;
        }
        else if (*p < 0xf0)
        {
            // трехбайтовый символ в UTF-8
            if (*(p + 1) < 0x80 || *(p + 1) > 0xbf || *(p + 2) < 0x80 || *(p + 2) > 0xbf)
            {
                // неправильные второй и/или третий байты, строка содержит ошибку
                return false;
            }
            p += 3;
        }
        else
        {
            // четырехбайтовый символ в UTF-8
            if (*(p + 1) < 0x80 || *(p + 1) > 0xbf || *(p + 2) < 0x80 || *(p + 2) > 0xbf || *(p + 3) < 0x80 || *(p + 3) > 0xbf)
            {
                // неправильные второй, третий и/или четвертый байты, строка содержит ошибку
                return false;
            }
            p += 4;
        }
    }
    return true;
}

#endif /* __XRCORE_STD_EXTENSIONS_H */
