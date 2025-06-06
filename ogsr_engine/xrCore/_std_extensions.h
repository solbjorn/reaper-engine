#ifndef __XRCORE_STD_EXTENSIONS_H
#define __XRCORE_STD_EXTENSIONS_H

#include <immintrin.h>

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
struct alignas(2 * sizeof(long)) xr_token
{
    LPCSTR name;
    long id;
};

IC LPCSTR get_token_name(const xr_token* tokens, int key)
{
    for (int k = 0; tokens[k].name; k++)
        if (key == tokens[k].id)
            return tokens[k].name;
    return "";
}

IC int get_token_id(xr_token* tokens, LPCSTR key)
{
    for (int k = 0; tokens[k].name; k++)
        if (stricmp(tokens[k].name, key) == 0)
            return tokens[k].id;
    return -1;
}

struct xr_token2
{
    LPCSTR name;
    LPCSTR info;
    int id;
};

// generic
template <class T>
IC T _min(T a, T b)
{
    return a < b ? a : b;
}
template <class T>
IC T _max(T a, T b)
{
    return a > b ? a : b;
}
template <class T>
IC T _sqr(T a)
{
    return a * a;
}

// float
IC float _abs(float x) { return fabsf(x); }
IC float _sqrt(float x) { return sqrtf(x); }
IC float _sin(float x) { return sinf(x); }
IC float _cos(float x) { return cosf(x); }
IC BOOL _valid(const float x)
{
    // check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
    int cls = _fpclass(double(x));
    if (cls & (_FPCLASS_SNAN + _FPCLASS_QNAN + _FPCLASS_NINF + _FPCLASS_PINF + _FPCLASS_ND + _FPCLASS_PD))
        return false;

    /*	*****other cases are*****
    _FPCLASS_NN Negative normalized non-zero
    _FPCLASS_NZ Negative zero ( – 0)
    _FPCLASS_PZ Positive 0 (+0)
    _FPCLASS_PN Positive normalized non-zero
    */
    return true;
}

// double
IC double _abs(double x) { return fabs(x); }
IC double _sqrt(double x) { return sqrt(x); }
IC double _sin(double x) { return sin(x); }
IC double _cos(double x) { return cos(x); }
IC BOOL _valid(const double x)
{
    // check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
    int cls = _fpclass(x);
    if (cls & (_FPCLASS_SNAN + _FPCLASS_QNAN + _FPCLASS_NINF + _FPCLASS_PINF + _FPCLASS_ND + _FPCLASS_PD))
        return false;

    /*	*****other cases are*****
    _FPCLASS_NN Negative normalized non-zero
    _FPCLASS_NZ Negative zero ( – 0)
    _FPCLASS_PZ Positive 0 (+0)
    _FPCLASS_PN Positive normalized non-zero
    */
    return true;
}

// int8
IC s8 _abs(s8 x) { return (x >= 0) ? x : s8(-x); }
IC s8 _min(s8 x, s8 y) { return y + ((x - y) & ((x - y) >> (sizeof(s8) * 8 - 1))); };
IC s8 _max(s8 x, s8 y) { return x - ((x - y) & ((x - y) >> (sizeof(s8) * 8 - 1))); };

// unsigned int8
IC u8 _abs(u8 x) { return x; }

// int16
IC s16 _abs(s16 x) { return (x >= 0) ? x : s16(-x); }
IC s16 _min(s16 x, s16 y) { return y + ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };
IC s16 _max(s16 x, s16 y) { return x - ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };

// unsigned int16
IC u16 _abs(u16 x) { return x; }

// int32
IC s32 _abs(s32 x) { return (x >= 0) ? x : s32(-x); }
IC s32 _min(s32 x, s32 y) { return y + ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };
IC s32 _max(s32 x, s32 y) { return x - ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };

// int64
IC s64 _abs(s64 x) { return (x >= 0) ? x : s64(-x); }
IC s64 _min(s64 x, s64 y) { return y + ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); };
IC s64 _max(s64 x, s64 y) { return x - ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); };

char* timestamp(string64& dest);

// Round @val to next or prev @mul boundary, where @mul is a power of two
constexpr ICF size_t roundup(size_t val, long mul) { return (val + mul - 1) & -mul; }
constexpr ICF size_t rounddown(size_t val, long mul) { return val & -mul; }

// Pack 64-bit value into <= 32 bits
// https://elixir.bootlin.com/linux/v6.13-rc3/source/include/linux/hash.h#L24
constexpr ICF u32 hash_64(const u64 val, const u32 bits)
{
#define GOLDEN_RATIO_64 0x61C8864680B583EBull
    return u32(val * GOLDEN_RATIO_64 >> (64 - bits));
}

constexpr ICF void xr_memcpy_const(void* dst, const void* src, size_t size)
{
    u8* cdst = (u8*)(dst);
    const u8* csrc = (const u8*)(src);

    for (size_t i = 0; i < size; i++)
        cdst[i] = csrc[i];
}

constexpr ICF void xr_memcpy16(void* dst, const void* src)
{
    if (std::is_constant_evaluated())
        xr_memcpy_const(dst, src, 16);
    else
        _mm_store_si128(reinterpret_cast<__m128i*>(dst), _mm_load_si128(reinterpret_cast<const __m128i*>(src)));
}

constexpr ICF void xr_memcpy128(void* dst, const void* src, size_t size)
{
    if (std::is_constant_evaluated())
        return xr_memcpy_const(dst, src, size);

    __m128i* cdst = reinterpret_cast<__m128i*>(dst);
    const __m128i* csrc = reinterpret_cast<const __m128i*>(src);

    for (size_t pos = 0; pos < size; pos += 16)
    {
        _mm_store_si128(cdst, _mm_load_si128(csrc));
        cdst++;
        csrc++;
    }
}

constexpr ICF void xr_memcpy32(void* dst, const void* src)
{
    if (std::is_constant_evaluated())
        xr_memcpy_const(dst, src, 32);
    else
        _mm256_store_si256(reinterpret_cast<__m256i*>(dst), _mm256_load_si256(reinterpret_cast<const __m256i*>(src)));
}

constexpr ICF void xr_memcpy256(void* dst, const void* src, size_t size)
{
    if (std::is_constant_evaluated())
        return xr_memcpy_const(dst, src, size);

    u8* cdst = reinterpret_cast<u8*>(dst);
    const u8* csrc = reinterpret_cast<const u8*>(src);

    do
    {
        _mm256_store_si256((__m256i*)cdst, _mm256_load_si256((const __m256i*)csrc));
        cdst += 32;
        csrc += 32;
        size -= 32;
    } while (size >= 32);
}

constexpr ICF void xr_memcpy(void* dst, const void* src, size_t size)
{
    if (std::is_constant_evaluated())
        return xr_memcpy_const(dst, src, size);

    u8* cdst = reinterpret_cast<u8*>(dst);
    const u8* csrc = reinterpret_cast<const u8*>(src);

    if (size >= 32 && !(((uintptr_t)dst | (uintptr_t)src) & 31))
    {
        do
        {
            _mm256_store_si256((__m256i*)cdst, _mm256_load_si256((const __m256i*)csrc));
            cdst += 32;
            csrc += 32;
            size -= 32;
        } while (size >= 32);
    }

    if (size >= 16 && !(((uintptr_t)dst | (uintptr_t)src) & 15))
    {
        do
        {
            _mm_store_si128((__m128i*)cdst, _mm_load_si128((const __m128i*)csrc));
            cdst += 16;
            csrc += 16;
            size -= 16;
        } while (size >= 16);
    }

    while (size >= 8)
    {
        *reinterpret_cast<u64*>(cdst) = *reinterpret_cast<const u64*>(csrc);
        cdst += 8;
        csrc += 8;
        size -= 8;
    }

    if (size >= 4)
    {
        *reinterpret_cast<u32*>(cdst) = *reinterpret_cast<const u32*>(csrc);
        cdst += 4;
        csrc += 4;
        size -= 4;
    }

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

IC char* xr_strlwr(char* S) { return strlwr(S); }

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
