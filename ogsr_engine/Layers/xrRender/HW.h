#ifndef AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#define AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_

// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#include <d3d11_4.h>
#include <dxgi1_4.h>

#include "../../xr_3da/context.h"

#include "hwcaps.h"
#include "stats_manager.h"

class CHW : public pureAppActivate, public pureAppDeactivate
{
    RTTI_DECLARE_TYPEINFO(CHW, pureAppActivate, pureAppDeactivate);

public:
    //	Functions section
    CHW();
    ~CHW() override;

    void CreateD3D();
    void DestroyD3D();
    tmc::task<void> CreateDevice(HWND wnd, u32& dwWidth, u32& dwHeight);

    void DestroyDevice();
    tmc::task<void> Reset(HWND wnd, u32& dwWidth, u32& dwHeight);

private:
    tmc::task<void> reset_st(HWND wnd);

    void selectResolution(u32& dwWidth, u32& dwHeight, BOOL bWindowed);
    u32 selectRefresh(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);
    void updateWindowProps(HWND hw);

public:
    void DumpVideoMemoryUsage() const;

    ICF ID3D11DeviceContext1* get_context(ctx_id_t context_id)
    {
        VERIFY(context_id < R__NUM_CONTEXTS);
        return contexts_pool[context_id];
    }

    ICF ID3D11DeviceContext1* get_imm_context() { return contexts_pool[R__IMM_CTX_ID]; }

    //	Variables section
    u32 BackBufferCount{};
    u32 CurrentBackBuffer{};

    IDXGIFactory2* m_pFactory{};
    IDXGIAdapter1* m_pAdapter{};
    ID3D11Device3* pDevice{}; // render device
    IDXGISwapChain1* m_pSwapChain{};

    CHWCaps Caps;
    DXGI_SWAP_CHAIN_DESC1 m_ChainDesc{}; // DevPP equivalent
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC m_ChainDescFullscreen{};
    HWND m_hWnd = nullptr;

    D3D_FEATURE_LEVEL FeatureLevel;
    bool m_SupportsVRR = false; // whether we can use DXGI_PRESENT_ALLOW_TEARING etc.
    bool DoublePrecisionFloatShaderOps;
    bool SAD4ShaderInstructions;
    bool ExtendedDoublesShaderInstructions;

private:
    IDXGIAdapter3* m_pAdapter3{};
    ID3D11DeviceContext1* contexts_pool[R__NUM_CONTEXTS]{};

public:
    stats_manager stats_manager{};

    bool doPresentTest{};

    void Present();
    DeviceState GetDeviceState();

    DXGI_RATIONAL selectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

    tmc::task<void> OnAppActivate() override;
    tmc::task<void> OnAppDeactivate() override;

private:
    void imgui_init() const;
    static void imgui_reset();
    static void imgui_shutdown();
};

extern CHW HW;

#endif // AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
