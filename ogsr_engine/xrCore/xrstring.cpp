#include "stdafx.h"

#include "xrstring.h"

#include <xxhash.h>
#pragma comment(lib, "xxhash")

XRCORE_API extern str_container* g_pStringContainer = NULL;

struct str_container_impl
{
    static constexpr u32 buffer_shift = 18;
    static constexpr size_t buffer_size = size_t(1) << buffer_shift;
    static_assert(buffer_size == 1024u * 256u);

    xrCriticalSection cs;
    str_value* buffer[buffer_size];
    int num_docs;

    str_container_impl()
    {
        num_docs = 0;
        ZeroMemory(buffer, sizeof(buffer));
    }

    str_value* find(str_value* value, const char* str) const
    {
        str_value* candidate = buffer[hash_64(value->dwXXH, buffer_shift)];
        while (candidate)
        {
            if (candidate->dwXXH == value->dwXXH && candidate->dwLength == value->dwLength && !memcmp(candidate->value, str, value->dwLength))
            {
                return candidate;
            }

            candidate = candidate->next;
        }

        return nullptr;
    }

    void insert(str_value* value)
    {
        str_value** element = &buffer[hash_64(value->dwXXH, buffer_shift)];
        value->next = *element;
        *element = value;
    }

    void clean()
    {
        for (size_t i = 0; i < buffer_size; ++i)
        {
            str_value** current = &buffer[i];

            while (*current != nullptr)
            {
                str_value* value = *current;
                if (!value->dwReference)
                {
                    *current = value->next;
                    xr_free(value);
                }
                else
                {
                    current = &value->next;
                }
            }
        }
    }

    void verify() const
    {
        Msg("strings verify started");
        for (size_t i = 0; i < buffer_size; ++i)
        {
            const str_value* value = buffer[i];
            while (value)
            {
                const XXH64_hash_t xxh = XXH3_64bits(value->value, value->dwLength);
                if (xxh != value->dwXXH)
                {
                    string32 xxh_str;
                    snprintf(xxh_str, 32, "0x%016llx", value->dwXXH);
                    R_ASSERT(xxh == value->dwXXH, "CorePanic: read-only memory corruption (shared_strings)", xxh_str);
                }
                R_ASSERT(value->dwLength == xr_strlen(value->value), "CorePanic: read-only memory corruption (shared_strings, internal structures)", value->value);
                value = value->next;
            }
        }
        Msg("strings verify completed");
    }

    void dump(FILE* f) const
    {
        for (size_t i = 0; i < buffer_size; ++i)
        {
            str_value* value = buffer[i];
            while (value)
            {
                fprintf(f, "ref[%4u]-len[%3u]-hash[0x%016llx] : %s\n", value->dwReference, value->dwLength, value->dwXXH, value->value);
                value = value->next;
            }
        }
    }

    void dump(IWriter* f) const
    {
        for (size_t i = 0; i < buffer_size; ++i)
        {
            str_value* value = buffer[i];
            string4096 temp;
            while (value)
            {
                xr_sprintf(temp, sizeof(temp), "ref[%4u]-len[%3u]-hash[0x%016llx] : %s\n", value->dwReference, value->dwLength, value->dwXXH, value->value);
                f->w_string(temp);
                value = value->next;
            }
        }
    }

    ptrdiff_t stat_economy() const
    {
        ptrdiff_t counter = 0;
        for (size_t i = 0; i < buffer_size; ++i)
        {
            const str_value* value = buffer[i];
            while (value)
            {
                counter -= sizeof(str_value);
                counter += (value->dwReference - 1) * (value->dwLength + 1);
                value = value->next;
            }
        }

        return counter;
    }
};

str_container::str_container() : impl(xr_new<str_container_impl>()) {}

str_value* str_container::dock(pcstr value) const
{
    if (nullptr == value)
        return nullptr;

    impl->cs.Enter();

    str_value* result = nullptr;

    // calc len
    const auto s_len = xr_strlen(value);
    const auto s_len_with_zero = s_len + 1;
    VERIFY(sizeof(str_value) + s_len_with_zero < 4096);

    // setup find structure
    char header[sizeof(str_value)];
    str_value* sv = (str_value*)header;
    sv->dwReference = 0;
    sv->dwLength = static_cast<u32>(s_len);
    sv->dwXXH = XXH3_64bits(value, s_len);

    // search
    result = impl->find(sv, value);

#ifdef DEBUG
    const bool is_leaked_string = !xr_strcmp(value, "enter leaked string here");
#endif // DEBUG

    // it may be the case, string is not found or has "non-exact" match
    if (nullptr == result
#ifdef DEBUG
        || is_leaked_string
#endif // DEBUG
    )
    {
        result = static_cast<str_value*>(xr_malloc(sizeof(str_value) + s_len_with_zero));

#ifdef DEBUG
        static int num_leaked_string = 0;
        if (is_leaked_string)
        {
            ++num_leaked_string;
            Msg("leaked_string: %d 0x%08x", num_leaked_string, result);
        }
#endif // DEBUG

        result->dwReference = 0;
        result->dwLength = sv->dwLength;
        result->dwXXH = sv->dwXXH;
        CopyMemory(result->value, value, s_len_with_zero);

        impl->insert(result);
    }
    impl->cs.Leave();

    return result;
}

void str_container::clean() const
{
    impl->cs.Enter();
    impl->clean();
    impl->cs.Leave();
}

void str_container::verify() const
{
    impl->cs.Enter();
    impl->verify();
    impl->cs.Leave();
}

void str_container::dump() const
{
    impl->cs.Enter();
    FILE* F = fopen("d:\\$str_dump$.txt", "w");
    impl->dump(F);
    fclose(F);
    impl->cs.Leave();
}

size_t str_container::stat_economy() const
{
    impl->cs.Enter();
    ptrdiff_t counter = 0;
    counter -= sizeof(*this);
    counter += impl->stat_economy();
    impl->cs.Leave();
    return size_t(counter);
}

str_container::~str_container()
{
    clean();
    // dump ();
    xr_delete(impl);
}
