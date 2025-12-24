#pragma once

#include <wrl.h>

// must conform to following order of allocation/free
// a(A), a(B), a(C), a(D), ....
// f(A), f(B), f(C), f(D), ....
// a(A), a(B), a(C), a(D), ....
//	this mean:
//		use as litle of queries as possible
//		first try to use queries allocated first
//	assumption:
//		used queries number is much smaller than total count

class R_occlusion
{
private:
    struct _Q
    {
        Microsoft::WRL::ComPtr<ID3DQuery> Q;
        u32 order;
        u32 ttl;
    };

    static constexpr u32 iInvalidHandle = 0xFFFFFFFF;

    xr_vector<_Q> pool; // sorted (max ... min), insertions are usually at the end
    xr_vector<_Q> used; // id's are generated from this and it is cleared from back only
    xr_vector<u32> fids; // free id's

    u32 last_frame;
    bool enabled;

    std::recursive_mutex lock;

    void cleanup_lost();

public:
    typedef u64 occq_result;

    ~R_occlusion();

    void occq_create();
    void occq_destroy(bool silent = false);

    u32 occq_begin(u32& ID, ctx_id_t context_id); // returns 'order'
    void occq_end(u32& ID, ctx_id_t context_id);
    occq_result occq_get(u32& ID);
    void occq_free(u32 ID);
};
