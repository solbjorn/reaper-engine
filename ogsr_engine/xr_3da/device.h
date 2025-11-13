#pragma once

#include "../xrcore/ftimer.h"
#include "pure.h"
#include "stats.h"

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/RenderDeviceRender.h"

#include <CallMe.h>

#include <atomic>
#include <numeric>

#define VIEWPORT_NEAR 0.2f
#define HUD_VIEWPORT_NEAR 0.05f

#define DEVICE_RESET_PRECACHE_FRAME_COUNT 10

class engine_impl;

class IRenderDevice
{};

class CRenderDeviceData
{
public:
    class CSecondVPParams //--#SM+#-- +SecondVP+
    {
    public:
        bool m_bCamReady; // Флаг готовности камеры (FOV, позиция, и т.п) к рендеру второго вьюпорта
    private:
        bool m_bIsActive; // Флаг активации рендера во второй вьюпорт
    public:
        IC bool IsSVPActive() { return m_bIsActive; }
        void SetSVPActive(bool bState);
        bool IsSVPFrame();
    };

public:
    u32 dwWidth;
    u32 dwHeight;

    u32 dwPrecacheFrame;
    BOOL b_is_Ready;
    BOOL b_is_Active;

public:
    // Engine flow-control
    u32 dwFrame;

    float fTimeDelta;
    float fTimeGlobal;
    u32 dwTimeDelta;
    u32 dwTimeGlobal;
    u32 dwTimeContinual;

    Fvector vCameraPosition;
    Fvector vCameraDirection;
    Fvector vCameraTop;
    Fvector vCameraRight;

    Fmatrix mView;
    Fmatrix mProject;
    Fmatrix mFullTransform;

    Fmatrix mViewHud;
    Fmatrix mProjectHud;

    Fmatrix mInvView;
    Fmatrix mInvFullTransform;

    // Copies of corresponding members. Used for synchronization.
    Fvector vCameraDirectionSaved;

    float fFOV;
    float fASPECT;

    // Генерирует псевдо-рандомное число в диапазоне от 0 до 1 https://stackoverflow.com/a/10625698
    template <typename T, size_t sizeDest>
    inline T NoiseRandom(const T (&args)[sizeDest]) const
    {
        constexpr double consts[]{23.14069263277926, 2.665144142690225};

        static_assert(sizeDest == std::size(consts));
        static_assert(std::is_floating_point_v<T>);

        constexpr auto frac = [](const double& x) {
            double res = x - std::floor(x);
            if constexpr (std::is_same_v<double, T>)
                return res;
            else
                return static_cast<T>(res);
        };

        return frac(std::cos(std::inner_product(std::begin(args), std::end(args), std::begin(consts), 0.0)) * 12345.6789);
    }

protected:
    s64 Timer_MM_Delta;
    CTimer_paused Timer;
    CTimer_paused TimerGlobal;

public:
    // Registrators
    MessageRegistry<pureRender> seqRender;
    MessageRegistry<pureAppActivate> seqAppActivate;
    MessageRegistry<pureAppDeactivate> seqAppDeactivate;
    MessageRegistry<pureAppStart> seqAppStart;
    MessageRegistry<pureAppEnd> seqAppEnd;
    MessageRegistry<pureFrame> seqFrame;
    MessageRegistry<pureScreenResolutionChanged> seqResolutionChanged;

    HWND m_hWnd;

    bool OnMainThread() const { return Core.OnMainThread(); }
};

class CRenderDeviceBase : public IRenderDevice, public CRenderDeviceData
{
public:
};

// refs
class CRenderDevice : public CRenderDeviceBase
{
private:
    u32 camFrame{std::numeric_limits<u32>::max()};

    // Main objects used for creating and rendering the 3D scene
    u32 m_dwWindowStyle;
    RECT m_rcWindowBounds;
    RECT m_rcWindowClient;

    CTimer TimerMM;

    void _Create();
    void _Destroy(BOOL bKeepTextures);
    void _SetupStates();

    xr_deque<CallMe::Delegate<void()>> seqParallel;

public:
    LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

    u32 dwPrecacheTotal;
    float fWidth_2, fHeight_2;
    void OnWM_Activate(WPARAM wParam);

public:
    IRenderDeviceRender* m_pRender{};

    BOOL m_bNearer;
    void SetNearer(BOOL enabled)
    {
        if (enabled && !m_bNearer)
        {
            m_bNearer = TRUE;
            mProject._43 -= EPS_L;
        }
        else if (!enabled && m_bNearer)
        {
            m_bNearer = FALSE;
            mProject._43 += EPS_L;
        }
        m_pRender->SetCacheXform(mView, mProject);
    }

    void DumpResourcesMemoryUsage() { m_pRender->ResourcesDumpMemoryUsage(); }

public:
    MessageRegistry<pureFrame> seqFrameMT;
    MessageRegistry<pureDeviceReset> seqDeviceReset;

    CSecondVPParams m_SecondViewport; //--#SM+#-- +SecondVP+

    CStats* Statistic;

    CRenderDevice()
    {
        m_hWnd = nullptr;
        b_is_Active = FALSE;
        b_is_Ready = FALSE;
        Timer.Start();
        m_bNearer = FALSE;

        //--#SM+#-- +SecondVP+
        m_SecondViewport.SetSVPActive(false);
        m_SecondViewport.m_bCamReady = false;
    }

    void Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason);
    BOOL Paused();

    // Scene control
    void ProcessFrame();
    void PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input);

    bool BeforeFrame();
    void FrameMove();
    void OnCameraUpdated();
    bool RenderBegin();
    void Clear();
    void RenderEnd();

    void overdrawBegin();
    void overdrawEnd();

    IC CTimer_paused* GetTimerGlobal() { return &TimerGlobal; }

    [[nodiscard]] auto TimerAsync() const { return TimerGlobal.GetElapsed_ms(); }
    [[nodiscard]] auto TimerAsync_MMT() const { return TimerMM.GetElapsed_ms() + Timer_MM_Delta; }

    // Creation & Destroying
    void ConnectToRender();
    void Create(void);
    void Run(void);
    void Destroy(void);
    void Reset(bool precache = true);

    void Initialize(void);

    void time_factor(const float& time_factor); //--#SM+#--

    inline float time_factor() const
    {
        VERIFY(fsimilar(Timer.time_factor(), TimerGlobal.time_factor()));
        return (Timer.time_factor());
    }

public:
    ICF bool add_to_seq_parallel(const CallMe::Delegate<void()>& delegate)
    {
        auto I = std::find(seqParallel.begin(), seqParallel.end(), delegate);
        if (I != seqParallel.end())
            return false;
        seqParallel.push_back(delegate);
        return true;
    }

    ICF void remove_from_seq_parallel(const CallMe::Delegate<void()>& delegate)
    {
        seqParallel.erase(std::remove(seqParallel.begin(), seqParallel.end(), delegate), seqParallel.end());
    }

private:
    void CalcFrameStats();

public:
    bool on_message(UINT uMsg, WPARAM wParam, LRESULT& result);

private:
    void message_loop();
};

extern CRenderDevice Device;
extern float refresh_rate;
extern bool g_bBenchmark;
extern xr_list<CallMe::Delegate<bool()>> g_loading_events;

class CLoadScreenRenderer : public pureRender
{
    RTTI_DECLARE_TYPEINFO(CLoadScreenRenderer, pureRender);

public:
    CLoadScreenRenderer();
    void start(bool b_user_input);
    void stop();
    virtual void OnRender();

    bool b_registered;
    bool b_need_user_input{};
};

// Device_wndproc.cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// device.cpp
extern BOOL bShowPauseString;
extern BOOL g_bLoaded;
extern BOOL g_bRendering;
extern CLoadScreenRenderer load_screen_renderer;

void GetMonitorResolution(u32& horizontal, u32& vertical);

// xr_ioc_cmd.cpp
extern u32 g_screenmode;
extern int ps_framelimiter;

extern float psHUD_FOV; //--#SM+#--
extern float psHUD_FOV_def; //--#SM+#--
