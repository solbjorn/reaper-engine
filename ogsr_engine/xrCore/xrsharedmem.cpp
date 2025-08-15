#include "stdafx.h"

#include <xxhash.h>

smem_container* g_pSharedMemoryContainer = NULL;

smem_value* smem_container::dock(u32 dwSize, void* ptr)
{
    VERIFY(dwSize && ptr);

    if (bDisable)
    {
        smem_value* result = (smem_value*)xr_malloc(sizeof(smem_value) + dwSize);
        result->dwReference = 0;
        result->dwSize = dwSize;
        result->dwXXH = XXH64_hash_t(-1);
        CopyMemory(result->value, ptr, dwSize);

        return result;
    }

    cs.Enter();
    smem_value* result = 0;

    XXH64_hash_t dwXXH = XXH3_64bits(ptr, dwSize);

    // search a place to insert
    u8 storage[sizeof(smem_value)];
    smem_value* value = (smem_value*)storage;
    value->dwReference = 0;
    value->dwXXH = dwXXH;
    value->dwSize = dwSize;
    cdb::iterator it = std::lower_bound(container.begin(), container.end(), value, smem_search);
    cdb::iterator saved_place = it;
    if (container.end() != it)
    {
        // supposedly found
        for (;; it++)
        {
            if (it == container.end())
                break;
            if ((*it)->dwXXH != dwXXH)
                break;
            if ((*it)->dwSize != dwSize)
                break;
            if (0 == memcmp((*it)->value, ptr, dwSize))
            {
                // really found
                result = *it;
                break;
            }
        }
    }

    // if not found - create new entry
    if (0 == result)
    {
        result = (smem_value*)xr_malloc(sizeof(smem_value) + dwSize);
        result->dwReference = 0;
        result->dwXXH = dwXXH;
        result->dwSize = dwSize;
        CopyMemory(result->value, ptr, dwSize);
        container.insert(saved_place, result);
    }

    // exit
    cs.Leave();
    return result;
}

void smem_container::clean()
{
    cs.Enter();

    for (auto& elem : container)
    {
        if (!elem->dwReference)
            xr_free(elem);
    }

    container.erase(std::remove(container.begin(), container.end(), static_cast<smem_value*>(nullptr)), container.end());
    if (container.empty())
        container.clear();

    cs.Leave();
}

void smem_container::dump()
{
    cs.Enter();
    cdb::iterator it = container.begin();
    cdb::iterator end = container.end();
    FILE* F = fopen("x:\\$smem_dump$.txt", "w");
    for (; it != end; it++)
        fprintf(F, "%4u : hash[0x%016llx], %u bytes\n", (*it)->dwReference, (*it)->dwXXH, (*it)->dwSize);
    fclose(F);
    cs.Leave();
}

u32 smem_container::stat_economy()
{
    cs.Enter();
    cdb::iterator it = container.begin();
    cdb::iterator end = container.end();
    s64 counter = 0;
    counter -= sizeof(*this);
    counter -= sizeof(cdb::allocator_type);
    const int node_size = 20;
    for (; it != end; it++)
    {
        counter -= 16;
        counter -= node_size;
        counter += s64((s64((*it)->dwReference) - 1) * s64((*it)->dwSize));
    }
    cs.Leave();

    return u32(s64(counter) / s64(1024));
}

smem_container::~smem_container() { clean(); }
