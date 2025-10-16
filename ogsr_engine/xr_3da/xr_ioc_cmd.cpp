#include "stdafx.h"

#include "igame_level.h"

#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_ioc_cmd.h"
#include "cameramanager.h"
#include "environment.h"
#include "xr_input.h"
#include "CustomHUD.h"
#include "../Include/xrRender/RenderDeviceRender.h"
#include "xr_object.h"
#include "SkeletonMotions.h"
#include "IGame_Persistent.h"
#include "LightAnimLibrary.h"

#include <regex>

xr_token* vid_quality_token = nullptr;

u32 g_screenmode = 1;
static constexpr xr_token screen_mode_tokens[] = {{"fullscreen", 2}, {"borderless", 1}, {"windowed", 0}, {nullptr, 0}};

#ifdef DEBUG
static constexpr xr_token vid_bpp_token[] = {{"16", 16}, {"32", 32}, {0, 0}};
#endif

void IConsole_Command::add_to_LRU(shared_str const& arg)
{
    if (arg.size() == 0 || bEmptyArgsHandled)
    {
        return;
    }

    bool dup = (std::find(m_LRU.begin(), m_LRU.end(), arg) != m_LRU.end());
    if (!dup)
    {
        m_LRU.push_back(arg);
        if (m_LRU.size() > LRU_MAX_COUNT)
        {
            m_LRU.erase(m_LRU.begin());
        }
    }
}

void IConsole_Command::add_LRU_to_tips(vecTips& tips)
{
    vecLRU::reverse_iterator it_rb = m_LRU.rbegin();
    vecLRU::reverse_iterator it_re = m_LRU.rend();
    for (; it_rb != it_re; ++it_rb)
    {
        tips.push_back((*it_rb));
    }
}

// =======================================================

class CCC_Quit : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Quit, IConsole_Command);

public:
    CCC_Quit(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override
    {
        Console->Hide();
        Engine.Event.Defer("KERNEL:disconnect");
        Engine.Event.Defer("KERNEL:quit");
    }
};

#ifdef DEBUG_MEMORY_MANAGER
class CCC_MemStat : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_MemStat, IConsole_Command);

public:
    CCC_MemStat(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR args) override
    {
        string_path fn;
        if (args && args[0])
            xr_sprintf(fn, sizeof(fn), "%s.dump", args);
        else
            strcpy_s_s(fn, sizeof(fn), "x:\\$memory$.dump");
        Memory.mem_statistic(fn);
        //		g_pStringContainer->dump			();
        //		g_pSharedMemoryContainer->dump		();
    }
};

class CCC_DbgMemCheck : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgMemCheck, IConsole_Command);

public:
    CCC_DbgMemCheck(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override
    {
        if (Memory.debug_mode)
            Memory.dbg_check();
        else
            Msg("~ Run with -mem_debug options.");
    }
};
#endif // DEBUG_MEMORY_MANAGER

class CCC_DbgStrCheck : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgStrCheck, IConsole_Command);

public:
    CCC_DbgStrCheck(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { g_pStringContainer->verify(); }
};

class CCC_DbgStrDump : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgStrDump, IConsole_Command);

public:
    CCC_DbgStrDump(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { g_pStringContainer->dump(); }
};

class CCC_DbgLALibDump : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgLALibDump, IConsole_Command);

public:
    CCC_DbgLALibDump(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { LALib.DbgDumpInfo(); }
};

//-----------------------------------------------------------------------
class CCC_MotionsStat : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_MotionsStat, IConsole_Command);

public:
    CCC_MotionsStat(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { g_pMotionsContainer->dump(); }
};

class CCC_TexturesStat : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_TexturesStat, IConsole_Command);

public:
    CCC_TexturesStat(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override
    {
        Device.DumpResourcesMemoryUsage();
        // Device.Resources->_DumpMemoryUsage();
        //	TODO: move this console commant into renderer
        // VERIFY(0);
    }
};
//-----------------------------------------------------------------------
class CCC_E_Dump : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_E_Dump, IConsole_Command);

public:
    CCC_E_Dump(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { Engine.Event.Dump(); }
};

class CCC_E_Signal : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_E_Signal, IConsole_Command);

public:
    CCC_E_Signal(LPCSTR N) : IConsole_Command{N} {}

    void Execute(LPCSTR args) override
    {
        char Event[128], Param[128];
        Event[0] = 0;
        Param[0] = 0;
        sscanf(args, "%[^,],%s", Event, Param);
        Engine.Event.Signal(Event, (u64)Param);
    }
};

//-----------------------------------------------------------------------
class CCC_Help : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Help, IConsole_Command);

public:
    CCC_Help(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override
    {
        Log("- --- Command listing: start ---");
        CConsole::vecCMD_IT it;
        for (it = Console->Commands.begin(); it != Console->Commands.end(); it++)
        {
            IConsole_Command& C = *(it->second);
            TStatus _S;
            C.Status(_S);
            TInfo _I;
            C.Info(_I);

            Msg("%-20s (%-10s) --- %s", C.Name(), _S, _I);
        }
        Log("Key: Ctrl + A         === Select all ");
        Log("Key: Ctrl + C         === Copy to clipboard ");
        Log("Key: Ctrl + V         === Paste from clipboard ");
        Log("Key: Ctrl + X         === Cut to clipboard ");
        Log("Key: Ctrl + Z         === Undo ");
        Log("Key: Ctrl + Insert    === Copy to clipboard ");
        Log("Key: Shift + Insert   === Paste from clipboard ");
        Log("Key: Shift + Delete   === Cut to clipboard ");
        Log("Key: Insert           === Toggle mode <Insert> ");
        Log("Key: Back / Delete          === Delete symbol left / right ");

        Log("Key: Up   / Down            === Prev / Next command in tips list ");
        Log("Key: Ctrl + Up / Ctrl + Down === Prev / Next executing command ");
        Log("Key: Left, Right, Home, End {+Shift/+Ctrl}       === Navigation in text ");
        Log("Key: PageUp / PageDown      === Scrolling history ");
        Log("Key: Tab  / Shift + Tab     === Next / Prev possible command from list");
        Log("Key: Enter  / NumEnter      === Execute current command ");

        Log("- --- Command listing: end ----");
    }
};

void _dump_open_files(int mode);

class CCC_DumpOpenFiles : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DumpOpenFiles, IConsole_Command);

public:
    CCC_DumpOpenFiles(LPCSTR N) : IConsole_Command{N} {}

    void Execute(LPCSTR args) override
    {
        int _mode = atoi(args);
        _dump_open_files(_mode);
    }
};

//-----------------------------------------------------------------------
class CCC_SaveCFG : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_SaveCFG, IConsole_Command);

public:
    CCC_SaveCFG(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR args) override
    {
        string_path cfg_full_name;
        xr_strcpy(cfg_full_name, (xr_strlen(args) > 0) ? args : Console->ConfigFile);

        bool b_abs_name = xr_strlen(cfg_full_name) > 2 && cfg_full_name[1] == ':';

        if (!b_abs_name)
            FS.update_path(cfg_full_name, "$app_data_root$", cfg_full_name);

        if (strext(cfg_full_name))
            *strext(cfg_full_name) = 0;
        xr_strcat(cfg_full_name, ".ltx");

        BOOL b_allow = TRUE;
        if (FS.exist(cfg_full_name))
            b_allow = SetFileAttributes(cfg_full_name, FILE_ATTRIBUTE_NORMAL);

        if (b_allow)
        {
            IWriter* F = FS.w_open(cfg_full_name);
            CConsole::vecCMD_IT it;
            for (it = Console->Commands.begin(); it != Console->Commands.end(); it++)
                it->second->Save(F);
            FS.w_close(F);
            Msg("Config-file [%s] saved successfully", cfg_full_name);
        }
        else
        {
            Msg("!Cannot store config file [%s]", cfg_full_name);
        }
    }
};

CCC_LoadCFG::CCC_LoadCFG(LPCSTR N) : IConsole_Command{N} {}

void CCC_LoadCFG::Execute(LPCSTR args)
{
    Msg("Executing config-script \"%s\"...", args);
    string_path cfg_name;

    xr_strcpy(cfg_name, args);
    if (strext(cfg_name))
        *strext(cfg_name) = 0;
    xr_strcat(cfg_name, ".ltx");

    string_path cfg_full_name;

    FS.update_path(cfg_full_name, "$app_data_root$", cfg_name);

    // if( NULL == FS.exist(cfg_full_name) )
    //	FS.update_path					(cfg_full_name, "$fs_root$", cfg_name);

    if (!FS.exist(cfg_full_name))
        xr_strcpy(cfg_full_name, cfg_name);

    IReader* F = FS.r_open(cfg_full_name);

    string1024 str;
    if (F)
    {
        while (!F->eof())
        {
            if (F->tell() == 0 && strstr(cfg_full_name, "user.ltx"))
            {
                // Костыль от ситуации когда в редких случаях почему-то у игроков бьётся user.ltx - оказывается набит нулями, в результате чего игра не
                // запускается. Не понятно почему так происходит, поэтому сделал тут обработку такой ситуации.

                if (F->elapsed() >= gsl::index{sizeof(u8)})
                {
                    if (F->r_u8() == 0)
                    {
                        Msg("!![%s] file [%s] broken!", __FUNCTION__, cfg_full_name);
                        FS.r_close(F);
                        FS.file_delete(cfg_full_name);
                        return;
                    }

                    F->seek(F->tell() - sizeof(u8));
                }
            }

            F->r_string(str, sizeof(str));
            if (allow(str))
                Console->Execute(str);
        }

        FS.r_close(F);
        Msg("[%s] successfully loaded.", cfg_full_name);
    }
    else
    {
        Msg("! Cannot open script file [%s]", cfg_full_name);
    }
}

CCC_LoadCFG_custom::CCC_LoadCFG_custom(LPCSTR cmd) : CCC_LoadCFG{cmd} { xr_strcpy(m_cmd, cmd); }

bool CCC_LoadCFG_custom::allow(LPCSTR cmd) { return (cmd == strstr(cmd, m_cmd)); }

//-----------------------------------------------------------------------
class CCC_Start : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Start, IConsole_Command);

private:
    std::string parse(const std::string& str)
    {
        static std::regex Reg("\\(([^)]+)\\)");
        std::smatch results;
        ASSERT_FMT(std::regex_search(str, results, Reg), "Failed parsing string: [%s]", str.c_str());
        return results[1].str();
    }

public:
    CCC_Start(const char* N) : IConsole_Command{N} {}

    void Execute(const char* args) override
    {
        auto str = parse(args);
        Engine.Event.Defer("KERNEL:start", u64(xr_strdup(str.c_str())), u64(xr_strdup("localhost")));
    }
};

class CCC_Disconnect : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Disconnect, IConsole_Command);

public:
    CCC_Disconnect(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { Engine.Event.Defer("KERNEL:disconnect"); }
};

//-----------------------------------------------------------------------
class CCC_VID_Reset : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_VID_Reset, IConsole_Command);

public:
    CCC_VID_Reset(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override
    {
        if (Device.b_is_Ready)
            Device.Reset();
    }
};

class CCC_VidMode : public CCC_Token
{
    RTTI_DECLARE_TYPEINFO(CCC_VidMode, CCC_Token);

private:
    u32 _dummy{};

public:
    CCC_VidMode(LPCSTR N) : CCC_Token{N, &_dummy, nullptr} { bEmptyArgsHandled = false; }

    void Execute(LPCSTR args) override
    {
        u32 _w, _h;
        int cnt = sscanf(args, "%ux%u", &_w, &_h);
        if (cnt == 2)
        {
            psCurrentVidMode[0] = _w;
            psCurrentVidMode[1] = _h;
        }
        else
        {
            Msg("! Wrong video mode [%s]", args);
            return;
        }
    }
    void Status(TStatus& S) override { xr_sprintf(S, sizeof(S), "%dx%d", psCurrentVidMode[0], psCurrentVidMode[1]); }
    const xr_token* GetToken() override { return vid_mode_token; }
    void Info(TInfo& I) override { xr_strcpy(I, sizeof(I), "change screen resolution WxH"); }

    void fill_tips(vecTips& tips) override
    {
        TStatus str, cur;
        Status(cur);

        bool res = false;
        const xr_token* tok = GetToken();
        while (tok->name && !res)
        {
            if (!xr_strcmp(tok->name, cur))
            {
                xr_sprintf(str, sizeof(str), "%s  (current)", tok->name);
                tips.push_back(str);
                res = true;
            }
            tok++;
        }
        if (!res)
        {
            tips.push_back("---  (current)");
        }
        tok = GetToken();
        while (tok->name)
        {
            tips.push_back(tok->name);
            tok++;
        }
    }
};

extern void GetMonitorResolution(u32& horizontal, u32& vertical);

class CCC_Screenmode : public CCC_Token
{
    RTTI_DECLARE_TYPEINFO(CCC_Screenmode, CCC_Token);

public:
    CCC_Screenmode(LPCSTR N) : CCC_Token{N, &g_screenmode, screen_mode_tokens} {}

    void Execute(LPCSTR args) override
    {
        u32 prev_mode = g_screenmode;
        CCC_Token::Execute(args);

        if ((prev_mode != g_screenmode))
        {
            // TODO: If you enable the debug layer for DX11 and switch between fullscreen and windowed a few times,
            // you'll occasionally see the following error in the output:
            // DXGI ERROR: IDXGISwapChain::Present: The application has not called ResizeBuffers or re-created the SwapChain after a fullscreen or windowed transition. Flip model
            // swapchains (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD) are required to do so. [ MISCELLANEOUS ERROR #117: ] This shouldn't happen if we're
            // calling reset on FS/Windowed transitions, I tried logging where Present() is called as well as where ResizeBuffers and SetFullscreenState are called, and the debug
            // output looks like this:
            //
            // Present()
            // SetFullscreenState()
            // [ERROR MESSAGE FROM ABOVE]
            // ResizeBuffers()
            // Present()
            //
            // Which makes no sense, it's impossible for us to have called Present between SetFullscreenState and ResizeBuffers
            // so DX11 must be doing it automatically
            //
            // These threads might be relevant for someone looking into this:
            // https://www.gamedev.net/forums/topic/652626-resizebuffers-bug/
            // https://www.gamedev.net/forums/topic/667426-dxgi-altenter-behaves-inconsistently/
            // https://www.gamedev.net/forums/topic/687484-correct-way-of-creating-the-swapchain-fullscreenwindowedvsync-about-refresh-rate/
            //
            // but the fixes make no sense and contradicts MSDN, and this isn't a major priority since ResizeBuffers is called
            // immediately after (before our Present call) so it works, just so stupid

            bool windowed_to_fullscreen = ((prev_mode == 0) || (prev_mode == 1)) && (g_screenmode == 2);
            bool fullscreen_to_windowed = (prev_mode == 2) && ((g_screenmode == 0) || (g_screenmode == 1));
            bool reset_required = windowed_to_fullscreen || fullscreen_to_windowed;
            if (Device.b_is_Ready && reset_required)
                Device.Reset();

            if (g_screenmode == 0 || g_screenmode == 1)
            {
                u32 w, h;
                GetMonitorResolution(w, h);
                SetWindowLongPtr(Device.m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
                SetWindowPos(Device.m_hWnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);

                if (g_screenmode == 0)
                    SetWindowLongPtr(Device.m_hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
            }
        }

        RECT winRect;
        GetClientRect(Device.m_hWnd, &winRect);
        MapWindowPoints(Device.m_hWnd, nullptr, reinterpret_cast<LPPOINT>(&winRect), 2);
        ClipCursor(&winRect);
    }
};
//-----------------------------------------------------------------------
class CCC_SND_Restart : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_SND_Restart, IConsole_Command);

public:
    CCC_SND_Restart(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { Sound->_restart(); }
};

//-----------------------------------------------------------------------
float ps_gamma = 1.f, ps_brightness = 1.f, ps_contrast = 1.f;

class CCC_Gamma : public CCC_Float
{
    RTTI_DECLARE_TYPEINFO(CCC_Gamma, CCC_Float);

public:
    CCC_Gamma(LPCSTR N, float* V) : CCC_Float(N, V, 0.5f, 1.5f) {}

    void Execute(LPCSTR args) override
    {
        CCC_Float::Execute(args);
        // Device.Gamma.Gamma		(ps_gamma);
        Device.m_pRender->setGamma(ps_gamma);
        // Device.Gamma.Brightness	(ps_brightness);
        Device.m_pRender->setBrightness(ps_brightness);
        // Device.Gamma.Contrast	(ps_contrast);
        Device.m_pRender->setContrast(ps_contrast);
        // Device.Gamma.Update		();
        Device.m_pRender->updateGamma();
    }
};

//-----------------------------------------------------------------------

class CCC_r2 : public CCC_Token
{
    RTTI_DECLARE_TYPEINFO(CCC_r2, CCC_Token);

private:
    typedef CCC_Token inherited;

    u32 renderer_value{};

public:
    CCC_r2(LPCSTR N) : inherited{N, &renderer_value, nullptr} {}
    virtual ~CCC_r2() = default;

    void Execute(LPCSTR args) override
    {
        //	vid_quality_token must be already created!
        tokens = vid_quality_token;

        inherited::Execute(args);
    }

    void Save(IWriter* F) override
    {
        tokens = vid_quality_token;
        inherited::Save(F);
    }

    const xr_token* GetToken() override
    {
        tokens = vid_quality_token;
        return inherited::GetToken();
    }

    void Status(TStatus& S) override
    {
        tokens = vid_quality_token;
        inherited::Status(S);
    }
};

class CCC_soundDevice : public CCC_Token
{
    RTTI_DECLARE_TYPEINFO(CCC_soundDevice, CCC_Token);

private:
    typedef CCC_Token inherited;

public:
    CCC_soundDevice(LPCSTR N) : inherited{N, &snd_device_id, nullptr} {}
    virtual ~CCC_soundDevice() {}

    void Execute(LPCSTR args) override
    {
        GetToken();
        if (!tokens)
            return;
        inherited::Execute(args);
    }

    void Status(TStatus& S) override
    {
        GetToken();
        if (!tokens)
            return;
        inherited::Status(S);
    }

    const xr_token* GetToken() override
    {
        tokens = snd_devices_token;
        return inherited::GetToken();
    }

    void Save(IWriter* F) override
    {
        GetToken();
        if (!tokens)
            return;
        inherited::Save(F);
    }
};

class CCC_ExclusiveMode : public CCC_Mask
{
    RTTI_DECLARE_TYPEINFO(CCC_ExclusiveMode, CCC_Mask);

private:
    using inherited = CCC_Mask;

public:
    CCC_ExclusiveMode(const char* N, Flags32* V, u32 M) : inherited{N, V, M} {}

    void Execute(const char* args) override
    {
        inherited::Execute(args);

        const bool val = GetValue();
        if (val != pInput->exclusive_mode())
            pInput->exclusive_mode(val);
    }
};

class CCC_HideConsole : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_HideConsole, IConsole_Command);

public:
    CCC_HideConsole(LPCSTR N) : IConsole_Command{N, true} {}

    void Execute(LPCSTR) override { Console->Hide(); }
    void Status(TStatus& S) override { S[0] = 0; }
    void Info(TInfo& I) override { xr_sprintf(I, sizeof(I), "hide console"); }
};

class CCC_SoundParamsSmoothing : public CCC_Integer
{
    RTTI_DECLARE_TYPEINFO(CCC_SoundParamsSmoothing, CCC_Integer);

public:
    CCC_SoundParamsSmoothing(LPCSTR N, int* V, int _min = 0, int _max = 999) : CCC_Integer{N, V, _min, _max} {}

    void Execute(LPCSTR args) override
    {
        CCC_Integer::Execute(args);
        soundSmoothingParams::alpha = soundSmoothingParams::getAlpha();
    }
};

float psHUD_FOV_def = 0.45f;
float psHUD_FOV = psHUD_FOV_def;

// extern int			psSkeletonUpdate;
extern int rsDVB_Size;
extern int rsDIB_Size;
// extern float		r__dtex_range;

extern int g_ErrorLineCount;

extern float g_fontWidthScale;
extern float g_fontHeightScale;

float ps_r2_sun_shafts_min = 0.f;
float ps_r2_sun_shafts_value = 1.f;

int ps_framelimiter = 0;

Fvector3 ssfx_wetness_multiplier{1.0f, 0.3f, 0.0f};

void CCC_Register()
{
    // General
    CMD1(CCC_Help, "help");
    CMD1(CCC_Quit, "quit");
    CMD1(CCC_Start, "start");
    CMD1(CCC_Disconnect, "disconnect");
    CMD1(CCC_SaveCFG, "cfg_save");
    CMD1(CCC_LoadCFG, "cfg_load");

    CMD1(CCC_MotionsStat, "stat_motions");
#ifdef DEBUG
    CMD1(CCC_TexturesStat, "stat_textures");
#endif // DEBUG

#ifdef DEBUG_MEMORY_MANAGER
    CMD1(CCC_MemStat, "dbg_mem_dump");
    CMD1(CCC_DbgMemCheck, "dbg_mem_check");
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG
    CMD3(CCC_Mask, "mt_particles", &psDeviceFlags, mtParticles);

    CMD1(CCC_DbgStrCheck, "dbg_str_check");
    CMD1(CCC_DbgStrDump, "dbg_str_dump");

    CMD3(CCC_Mask, "mt_sound", &psDeviceFlags, mtSound);
    CMD3(CCC_Mask, "mt_physics", &psDeviceFlags, mtPhysics);
    CMD3(CCC_Mask, "mt_network", &psDeviceFlags, mtNetwork);

    // Events
    CMD1(CCC_E_Dump, "e_list");
    CMD1(CCC_E_Signal, "e_signal");

    CMD3(CCC_Mask, "rs_wireframe", &psDeviceFlags, rsWireframe);
    CMD3(CCC_Mask, "rs_clear_bb", &psDeviceFlags, rsClearBB);
    CMD3(CCC_Mask, "rs_occlusion", &psDeviceFlags, rsOcclusion);

    CMD3(CCC_Mask, "rs_detail", &psDeviceFlags, rsDetails);
    // CMD4(CCC_Float,		"r__dtex_range",		&r__dtex_range,		5,		175	);

    CMD3(CCC_Mask, "rs_render_statics", &psDeviceFlags, rsDrawStatic);
    CMD3(CCC_Mask, "rs_render_dynamics", &psDeviceFlags, rsDrawDynamic);
#endif

    // Render device states
    CMD3(CCC_Mask, "rs_always_active", &psDeviceFlags, rsAlwaysActive);

    CMD4(CCC_Float, "r2_sunshafts_min", &ps_r2_sun_shafts_min, 0.0, 0.5);
    CMD4(CCC_Float, "r2_sunshafts_value", &ps_r2_sun_shafts_value, 0.5, 2.0);

    CMD3(CCC_Mask, "rs_v_sync", &psDeviceFlags, rsVSync);
    //	CMD3(CCC_Mask,		"rs_disable_objects_as_crows",&psDeviceFlags,	rsDisableObjectsAsCrows	);
    CMD1(CCC_Screenmode, "rs_screenmode");
    CMD3(CCC_Mask, "rs_stats", &psDeviceFlags, rsStatistic);
    CMD4(CCC_Float, "rs_vis_distance", &psVisDistance, 0.4f, 1.5f);

    CMD3(CCC_Mask, "rs_cam_pos", &psDeviceFlags, rsCameraPos);
#ifdef DEBUG
    CMD3(CCC_Mask, "rs_occ_draw", &psDeviceFlags, rsOcclusionDraw);
    CMD3(CCC_Mask, "rs_occ_stats", &psDeviceFlags, rsOcclusionStats);
    // CMD4(CCC_Integer,	"rs_skeleton_update",	&psSkeletonUpdate,	2,		128	);
#endif // DEBUG

    // Вместо этих настроек теперь используется ES Color Grading
    //     CMD2(CCC_Gamma, "rs_c_gamma", &ps_gamma);
    //     CMD2(CCC_Gamma, "rs_c_brightness", &ps_brightness);
    //     CMD2(CCC_Gamma, "rs_c_contrast", &ps_contrast);

    //	CMD4(CCC_Integer,	"rs_vb_size",			&rsDVB_Size,		32,		4096);
    //	CMD4(CCC_Integer,	"rs_ib_size",			&rsDIB_Size,		32,		4096);

    CMD3(CCC_Mask, "rs_hw_stats", &psDeviceFlags, rsHWInfo);

    // General video control
    CMD1(CCC_VidMode, "vid_mode");

    CMD1(CCC_VID_Reset, "vid_restart");

    // Sound
    CMD2(CCC_Float, "snd_volume_eff", &psSoundVEffects);
    CMD2(CCC_Float, "snd_volume_music", &psSoundVMusic);
    CMD1(CCC_SND_Restart, "snd_restart");

    CMD3(CCC_Mask, "snd_efx", &psSoundFlags, ss_EFX);
    CMD3(CCC_Mask, "snd_use_float32", &psSoundFlags, ss_UseFloat32);
    CMD4(CCC_Integer, "snd_targets", &psSoundTargets, 128, 1024);

    CMD4(CCC_Float, "snd_rolloff", &psSoundRolloff, 0.1f, 2.f);
    CMD4(CCC_Float, "snd_fade_speed", &psSoundFadeSpeed, 1.f, 10.f);
    CMD4(CCC_Float, "snd_occ_scale", &psSoundOcclusionScale, 0.1f, 1.f);
    CMD4(CCC_Float, "snd_occ_hf", &psSoundOcclusionHf, 0.f, 1.f);
    CMD4(CCC_Float, "snd_occ_mtl", &psSoundOcclusionMtl, 0.f, 1.f);

    // Doppler effect power
    CMD4(CCC_Float, "snd_doppler_power", &soundSmoothingParams::power, 0.f, 5.f);
    CMD4(CCC_SoundParamsSmoothing, "snd_doppler_smoothing", &soundSmoothingParams::steps, 1, 100);

#ifdef DEBUG
    CMD3(CCC_Mask, "snd_stats", &g_stats_flags, st_sound);
    CMD3(CCC_Mask, "snd_stats_min_dist", &g_stats_flags, st_sound_min_dist);
    CMD3(CCC_Mask, "snd_stats_max_dist", &g_stats_flags, st_sound_max_dist);
    CMD3(CCC_Mask, "snd_stats_ai_dist", &g_stats_flags, st_sound_ai_dist);
    CMD3(CCC_Mask, "snd_stats_info_name", &g_stats_flags, st_sound_info_name);
    CMD3(CCC_Mask, "snd_stats_info_object", &g_stats_flags, st_sound_info_object);

    CMD4(CCC_Integer, "error_line_count", &g_ErrorLineCount, 6, 1024);
#endif // DEBUG

    // Mouse
    CMD3(CCC_Mask, "mouse_invert", &psMouseInvert, 1);
    psMouseSens = 0.12f;
    CMD4(CCC_Float, "mouse_sens", &psMouseSens, 0.001f, 0.6f);

    // Camera
    CMD4(CCC_Float, "cam_inert", &psCamInert, 0.0f, 0.99f);
    CMD2(CCC_Float, "cam_slide_inert", &psCamSlideInert);

    CMD1(CCC_r2, "renderer");

    CMD1(CCC_soundDevice, "snd_device");

#ifdef DEBUG
    CMD1(CCC_DumpOpenFiles, "dump_open_files");
#endif

    CMD3(CCC_ExclusiveMode, "input_exclusive_mode", &psDeviceFlags, rsExclusiveMode);

    // extern int g_svTextConsoleUpdateRate;
    // CMD4(CCC_Integer, "sv_console_update_rate", &g_svTextConsoleUpdateRate, 1, 100);

    CMD1(CCC_HideConsole, "hide");

    CMD4(CCC_Integer, "r__framelimit", &ps_framelimiter, 0, 500);

    CMD4(CCC_Vector3, "ssfx_wetness_multiplier", &ssfx_wetness_multiplier, Fvector3().set(0.1f, 0.1f, 0.0f), Fvector3().set(20.0f, 20.0f, 0.0f));

#ifdef DEBUG
    extern BOOL debug_destroy;
    CMD4(CCC_Integer, "debug_destroy", &debug_destroy, FALSE, TRUE);
#endif

    CMD4(CCC_Float, "g_font_scale_x", &g_fontWidthScale, 0.2f, 5.0f);
    CMD4(CCC_Float, "g_font_scale_y", &g_fontHeightScale, 0.2f, 5.0f);

    CMD4(CCC_Integer, "g_prefetch", &g_prefetch, 0, 1);

    CMD1(CCC_DbgLALibDump, "dbg_lalib_dump");
}
