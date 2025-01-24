#ifndef dx10StateCache_included
#define dx10StateCache_included

template <class IDeviceState, class StateDecs>
class dx10StateCache
{
    //	Public interface
public:
    dx10StateCache();
    ~dx10StateCache();

    void ClearStateArray();

    IDeviceState* GetState(SimulatorStates& state_code);
    IDeviceState* GetState(StateDecs& desc);
    //	Can be called on device destruction only!
    //	dx10State holds weak links on manager's states and
    //	won't understand that state was destroyed
    // void	FlushStates();
    //	Private functionality

    //	Private declarations
private:
    struct StateRecord
    {
        u64 m_xxh;
        IDeviceState* m_pState;
    };

private:
    void CreateState(StateDecs desc, IDeviceState** ppIState);
    IDeviceState* FindState(const StateDecs& desc, u64 StateXXH);

    //	Private data
private:
    //	This must be cleared on device destroy
    xr_vector<StateRecord> m_StateArray;
};

extern dx10StateCache<ID3DRasterizerState, D3D_RASTERIZER_DESC> RSManager;
extern dx10StateCache<ID3DDepthStencilState, D3D_DEPTH_STENCIL_DESC> DSSManager;
extern dx10StateCache<ID3DBlendState, D3D_BLEND_DESC> BSManager;

#include "dx10StateCacheImpl.h"

#endif //	dx10StateCache_included
