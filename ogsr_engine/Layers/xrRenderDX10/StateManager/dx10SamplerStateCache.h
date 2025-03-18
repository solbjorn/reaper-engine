#ifndef dx10SamplerStateCache_included
#define dx10SamplerStateCache_included

class dx10SamplerStateCache
{
public:
    enum
    {
        hInvalidHandle = 0xFFFFFFFF
    };

    //	State handle
    typedef u32 SHandle;
    typedef xr_vector<SHandle> HArray;

public:
    dx10SamplerStateCache();
    ~dx10SamplerStateCache();

    void ClearStateArray();

    SHandle GetState(D3D_SAMPLER_DESC& desc);

    void VSApplySamplers(u32 context_id, HArray& samplers);
    void PSApplySamplers(u32 context_id, HArray& samplers);
    void GSApplySamplers(u32 context_id, HArray& samplers);
    void HSApplySamplers(u32 context_id, HArray& samplers);
    void DSApplySamplers(u32 context_id, HArray& samplers);
    void CSApplySamplers(u32 context_id, HArray& samplers);

    void SetMaxAnisotropy(u32 uiMaxAniso);
    void SetMipLODBias(float uiMipLODBias);

private:
    typedef ID3DSamplerState IDeviceState;
    typedef D3D_SAMPLER_DESC StateDecs;

    struct StateRecord
    {
        u64 m_xxh{};
        IDeviceState* m_pState{};
    };

private:
    void CreateState(StateDecs desc, IDeviceState** ppIState);
    SHandle FindState(const StateDecs& desc, u64 StateXXH);

    void PrepareSamplerStates(HArray& samplers, ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT]) const;

    //	Private data
private:
    //	This must be cleared on device destroy
    xr_vector<StateRecord> m_StateArray;

    u32 m_uiMaxAnisotropy;
    float m_uiMipLODBias;
};

extern dx10SamplerStateCache SSManager;

#endif //	dx10SamplerStateCache_included
