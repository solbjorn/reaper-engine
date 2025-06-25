#ifndef SH_RT_H
#define SH_RT_H

//////////////////////////////////////////////////////////////////////////
class CRT : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(CRT, xr_resource_named);

public:
    enum : u32 // extends xr_resource_flagged flags
    {
        /*RF_REGISTERED = xr_resource_flagged::RF_REGISTERED,*/
        CreateUAV = 1 << 1, // Self descriptive. DX11-specific.
        /* 1 << 2, */
        CreateBase = 1 << 3, // Creates basic RTV from backbuffer or DSV (depending on format)
    };

    CRT() = default;
    ~CRT();
    void create(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 SampleCount = 1, u32 slices_num = 1, Flags32 flags = {});
    void destroy();
    void reset_begin();
    void reset_end();
    BOOL valid() { return !!pTexture; }

    void set_slice_read(int slice);
    void set_slice_write(ctx_id_t context_id, int slice);

    ID3DTexture2D* pSurface{};
    ID3DRenderTargetView* pRT{};
    ID3DDepthStencilView* pZRT[R__NUM_CONTEXTS]{};
    ID3DDepthStencilView* dsv_all{};
    xr_vector<ID3DDepthStencilView*> dsv_per_slice;
    ID3D11UnorderedAccessView* pUAView{};

    ref_texture pTexture;

    u32 dwWidth{};
    u32 dwHeight{};
    D3DFORMAT fmt{};
    u32 sampleCount{};
    u32 n_slices{};

    u64 _order{};
};

struct resptrcode_crt : public resptr_base<CRT>
{
    void create(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 SampleCount = 1, Flags32 flags = {}) { create(Name, w, h, f, SampleCount, 1, flags); }
    void create(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 SampleCount, u32 slices_num, Flags32 flags);
    void destroy() { _set(nullptr); }
};
typedef resptr_core<CRT, resptrcode_crt> ref_rt;

#endif // SH_RT_H
