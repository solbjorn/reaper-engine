#ifndef xrPoolH
#define xrPoolH

template <typename T, gsl::index granularity>
class poolSS
{
    T* list{};
    xr_vector<T*> blocks;

    [[nodiscard]] T** access(T* P) { return reinterpret_cast<T**>(P); }

    void block_create()
    {
        constexpr gsl::index count{granularity - 1}; // minus one. Correct. See partition logic below.

        // Allocate
        VERIFY(list == nullptr);

        list = xr_alloc<T>(granularity);
        blocks.push_back(list);

        // Partition
        for (gsl::index it{}; it < count; ++it)
        {
            T* E = list + it;
            *access(E) = E + 1;
        }

        *access(list + count) = nullptr;
    }

public:
    poolSS() = default;

    ~poolSS()
    {
        for (auto& block : blocks)
            xr_free(block);
    }

    [[nodiscard]] T* create()
    {
        if (list == nullptr)
            block_create();

        T* E = list;
        list = *access(list);

        return new (E) T{};
    }

    void destroy(T*& P)
    {
        P->~T();
        *access(P) = list;

        list = P;
        P = nullptr;
    }

    void clear()
    {
        list = nullptr;

        for (auto& block : blocks)
            xr_free(block);

        blocks.clear();
    }
};

#endif
