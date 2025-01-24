#ifndef __RAPIDHASH_MSVC_H
#define __RAPIDHASH_MSVC_H

#include <rapidhash/rapidhash.h>

constexpr RAPIDHASH_INLINE void rapid_mum_const_msvc(u64* A, u64* B) RAPIDHASH_NOEXCEPT
{
    u64 ha = *A >> 32, hb = *B >> 32, la = (u32)*A, lb = (u32)*B, hi, lo;
    u64 rh = ha * hb, rm0 = ha * lb, rm1 = hb * la, rl = la * lb, t = rl + (rm0 << 32), c = t < rl;

    lo = t + (rm1 << 32);
    c += lo < t;
    hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;

#ifdef RAPIDHASH_PROTECTED
    *A ^= lo;
    *B ^= hi;
#else
    *A = lo;
    *B = hi;
#endif
}

constexpr RAPIDHASH_INLINE void rapid_mum_msvc(u64* A, u64* B) RAPIDHASH_NOEXCEPT
{
    if (std::is_constant_evaluated())
        rapid_mum_const_msvc(A, B);
    else
        rapid_mum(A, B);
}

constexpr RAPIDHASH_INLINE u64 rapid_mix_msvc(u64 A, u64 B) RAPIDHASH_NOEXCEPT
{
    rapid_mum_msvc(&A, &B);

    return A ^ B;
}

#ifdef RAPIDHASH_LITTLE_ENDIAN
RAPIDHASH_INLINE u64 rapid_read64_msvc(const u8* p) RAPIDHASH_NOEXCEPT { return *reinterpret_cast<const u64*>(p); }
RAPIDHASH_INLINE u64 rapid_read32_msvc(const u8* p) RAPIDHASH_NOEXCEPT { return *reinterpret_cast<const u32*>(p); }
#endif

constexpr RAPIDHASH_INLINE u64 rapidhash_internal_msvc(const void* key, size_t len, u64 seed, const u64* secret) RAPIDHASH_NOEXCEPT
{
    const u8* p = (const u8*)key;
    seed ^= rapid_mix_msvc(seed ^ secret[0], secret[1]) ^ len;
    u64 a, b;

    if (len <= 16) [[likely]]
    {
        if (len >= 4) [[likely]]
        {
            const u8* plast = p + len - 4;
            a = (rapid_read32_msvc(p) << 32) | rapid_read32_msvc(plast);

            const u64 delta = ((len & 24) >> (len >> 3));
            b = ((rapid_read32_msvc(p + delta) << 32) | rapid_read32_msvc(plast - delta));
        }
        else if (len > 0) [[likely]]
        {
            a = rapid_readSmall(p, len);
            b = 0;
        }
        else
            a = b = 0;
    }
    else
    {
        size_t i = len;

        if (i > 48) [[unlikely]]
        {
            u64 see1 = seed, see2 = seed;

#ifdef RAPIDHASH_UNROLLED
            while (i >= 96) [[likely]]
            {
                seed = rapid_mix_msvc(rapid_read64_msvc(p) ^ secret[0], rapid_read64_msvc(p + 8) ^ seed);
                see1 = rapid_mix_msvc(rapid_read64_msvc(p + 16) ^ secret[1], rapid_read64_msvc(p + 24) ^ see1);
                see2 = rapid_mix_msvc(rapid_read64_msvc(p + 32) ^ secret[2], rapid_read64_msvc(p + 40) ^ see2);
                seed = rapid_mix_msvc(rapid_read64_msvc(p + 48) ^ secret[0], rapid_read64_msvc(p + 56) ^ seed);
                see1 = rapid_mix_msvc(rapid_read64_msvc(p + 64) ^ secret[1], rapid_read64_msvc(p + 72) ^ see1);
                see2 = rapid_mix_msvc(rapid_read64_msvc(p + 80) ^ secret[2], rapid_read64_msvc(p + 88) ^ see2);
                p += 96;
                i -= 96;
            }

            if (i >= 48) [[unlikely]]
            {
                seed = rapid_mix_msvc(rapid_read64_msvc(p) ^ secret[0], rapid_read64_msvc(p + 8) ^ seed);
                see1 = rapid_mix_msvc(rapid_read64_msvc(p + 16) ^ secret[1], rapid_read64_msvc(p + 24) ^ see1);
                see2 = rapid_mix_msvc(rapid_read64_msvc(p + 32) ^ secret[2], rapid_read64_msvc(p + 40) ^ see2);

                p += 48;
                i -= 48;
            }
#else
            do
            {
                seed = rapid_mix_msvc(rapid_read64_msvc(p) ^ secret[0], rapid_read64_msvc(p + 8) ^ seed);
                see1 = rapid_mix_msvc(rapid_read64_msvc(p + 16) ^ secret[1], rapid_read64_msvc(p + 24) ^ see1);
                see2 = rapid_mix_msvc(rapid_read64_msvc(p + 32) ^ secret[2], rapid_read64_msvc(p + 40) ^ see2);

                p += 48;
                i -= 48;
            } while (i >= 48) [[likely]];
#endif

            seed ^= see1 ^ see2;
        }

        if (i > 16)
        {
            seed = rapid_mix_msvc(rapid_read64_msvc(p) ^ secret[2], rapid_read64_msvc(p + 8) ^ seed ^ secret[1]);
            if (i > 32)
                seed = rapid_mix_msvc(rapid_read64_msvc(p + 16) ^ secret[2], rapid_read64_msvc(p + 24) ^ seed);
        }

        a = rapid_read64_msvc(p + i - 16);
        b = rapid_read64_msvc(p + i - 8);
    }

    a ^= secret[1];
    b ^= seed;
    rapid_mum_msvc(&a, &b);

    return rapid_mix_msvc(a ^ secret[0] ^ len, b ^ secret[1]);
}

constexpr RAPIDHASH_INLINE u64 rapidhash_msvc(const void* key, size_t len) RAPIDHASH_NOEXCEPT { return rapidhash_internal_msvc(key, len, RAPID_SEED, rapid_secret); }
constexpr RAPIDHASH_INLINE u64 rapidhash_msvc(u64 key) RAPIDHASH_NOEXCEPT { return rapidhash_msvc(&key, sizeof(key)); }

#endif /* __RAPIDHASH_MSVC_H */
