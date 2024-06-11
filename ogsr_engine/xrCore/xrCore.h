#pragma once

// KRodin: это инклудить только здесь и нигде больше!
#if __has_include("..\build_config_overrides\build_config_defines.h")
#include "..\build_config_overrides\build_config_defines.h"
#else
#include "..\build_config_defines.h"
#endif

#if defined(__MSVC_RUNTIME_CHECKS) && defined(__SANITIZE_ADDRESS__)
#error DISABLE RTC!
#endif

#ifndef _MT // multithreading disabled
#error Please enable multi-threaded library...
#endif

#if defined(_DEBUG) && !defined(DEBUG) // Visual Studio defines _DEBUG when you specify the /MTd or /MDd option
#define DEBUG
#endif

#if defined(_DEBUG) && defined(NDEBUG)
#error Something strange...
#endif

#if defined(DEBUG) && defined(NDEBUG)
#error Something strange...
#endif

#if defined(_DEBUG) && defined(DISABLE_DBG_ASSERTIONS)
#define NDEBUG
#undef DEBUG
#endif

#ifndef DEBUG
#define MASTER_GOLD
#endif

#include "xrCore_platform.h"

#define IC inline
#define ICF __forceinline // !!! this should be used only in critical places found by PROFILER
#define ICN __declspec(noinline)

// Require function-like macros to end with a ';'
#define XR_MACRO_END() static_assert(true, "")

#define __XR_DIAG_STR(s) #s
#define XR_DIAG_PUSH() _Pragma(__XR_DIAG_STR(clang diagnostic push)) XR_MACRO_END()
#define XR_DIAG_IGNORE(s) _Pragma(__XR_DIAG_STR(clang diagnostic ignored s)) XR_MACRO_END()
#define XR_DIAG_POP() _Pragma(__XR_DIAG_STR(clang diagnostic pop)) XR_MACRO_END()

#ifdef _MSC_VER
#define XR_NOVTABLE __declspec(novtable)
#else
#define XR_NOVTABLE
#endif

#define XR_PRINTF(a, b) __attribute__((__format__(printf, a, b)))

// Treat the class as trivial even if it has user-defined constructors and/or
// assignment operators, e.g. the ones using AVX
#define XR_TRIVIAL [[clang::trivial_abi]]

// As of LLVM 21, [[clang::trivial_abi]] with MSVC ABI barely works:
// * doesn't work when inheriting a trivial base
// * doesn't work when a class has a user-defined move
// * std::is_trivially_copyable_v<T> still returns false
// See also:
// https://github.com/llvm/llvm-project/issues/59131
// https://github.com/llvm/llvm-project/issues/69394
// https://github.com/llvm/llvm-project/issues/87993
// https://github.com/llvm/llvm-project/pull/88857
#define XR_TRIVIAL_BROKEN

#define XR_TRIVIAL_ASSERT(type, ...) static_assert(__builtin_is_cpp_trivially_relocatable(type, ##__VA_ARGS__))

// Our headers

#include "xrDebug.h"
#include "vector.h"

#include "clsid.h"
#include "xr_rtti.h"
#include "xrSyncronize.h"
#include "xrMemory.h"

#include "_stl_extensions.h"
#include "log.h"
#include "xrsharedmem.h"
#include "xrstring.h"
#include "xr_resource.h"

DEFINE_VECTOR(shared_str, RStringVec, RStringVecIt);

#include "FS.h"
#include "xr_trims.h"
#include "xr_ini.h"

#if defined(_DEBUG) || defined(OGSR_TOTAL_DBG)
#define LogDbg Log
#define MsgDbg Msg
#define FuncDbg(...) __VA_ARGS__
#define LOG_SECOND_THREAD_STATS
#else
#define LogDbg __noop
#define MsgDbg __noop
#define FuncDbg __noop
#endif

#ifdef OGSR_TOTAL_DBG
#define ASSERT_FMT_DBG ASSERT_FMT
#else
#define ASSERT_FMT_DBG(cond, ...) \
    do \
    { \
        if (!(cond)) \
            Msg(__VA_ARGS__); \
    } while (0) // Вылета не будет, просто в лог напишем
#endif

#include "LocatorAPI.h"
#include "FTimer.h"
#include "intrusive_ptr.h"

// ********************************************** The Core definition
class xrCore
{
public:
    string64 ApplicationName;
    string_path ApplicationPath;
    string_path WorkingPath;
    string64 UserName;
    string64 CompName;
    string512 Params;

    Flags16 ParamFlags;
    enum ParamFlag
    {
        dbg = (1 << 0),
    };

    Flags64 Features{};
    struct Feature
    {
        static constexpr u64 equipped_untradable = 1ull << 0, highlight_equipped = 1ull << 1, af_radiation_immunity_mod = 1ull << 2, condition_jump_weight_mod = 1ull << 3,
                             forcibly_equivalent_slots = 1ull << 4, slots_extend_menu = 1ull << 5, dynamic_sun_movement = 1ull << 6, wpn_bobbing = 1ull << 7,
                             show_inv_item_condition = 1ull << 8, remove_alt_keybinding = 1ull << 9, binoc_firing = 1ull << 10, new_pda_info = 1ull << 11,
                             stop_anim_playing = 1ull << 12, corpses_collision = 1ull << 13, more_hide_weapon = 1ull << 14, keep_inprogress_tasks_only = 1ull << 15,
                             show_dialog_numbers = 1ull << 16, objects_radioactive = 1ull << 17, af_zero_condition = 1ull << 18, af_satiety = 1ull << 19,
                             af_psy_health = 1ull << 20, outfit_af = 1ull << 21, gd_master_only = 1ull << 22, scope_textures_autoresize = 1ull << 23, ogse_new_slots = 1ull << 24,
                             ogse_wpn_zoom_system = 1ull << 25, wpn_cost_include_addons = 1ull << 26, /* = 1ull << 27,*/ hard_ammo_reload = 1ull << 28,
                             engine_ammo_repacker = 1ull << 29, ruck_flag_preferred = 1ull << 30, colorize_ammo = 1ull << 31, pickup_bolts = 1ull << 32,
                             colorize_untradable = 1ull << 33, select_mode_1342 = 1ull << 34, old_outfit_slot_style = 1ull << 35, npc_simplified_shooting = 1ull << 36,
                             autoreload_wpn = 1ull << 37, use_trade_deficit_factor = 1ull << 38, show_objectives_ondemand = 1ull << 39, pickup_check_overlaped = 1ull << 40,
                             disable_dialog_break = 1ull << 41, actor_thirst = 1ull << 42, no_progress_bar_animation = 1ull << 43;
    };

private:
    std::thread::id mainThreadId;

public:
    bool OnMainThread() const { return std::this_thread::get_id() == mainThreadId; }

    void _initialize(LPCSTR ApplicationName, LogCallback cb = nullptr, BOOL init_fs = TRUE, LPCSTR fs_fname = nullptr);
    void _destroy();

    constexpr const char* GetBuildConfiguration();
    const char* GetEngineVersion();
};

extern xrCore Core;
extern bool gModulesLoaded;

// Трэш
#define BENCH_SEC_CALLCONV
#define BENCH_SEC_SCRAMBLEVTBL1
#define BENCH_SEC_SCRAMBLEVTBL2
#define BENCH_SEC_SCRAMBLEVTBL3
#define BENCH_SEC_SIGN
#define BENCH_SEC_SCRAMBLEMEMBER1
#define BENCH_SEC_SCRAMBLEMEMBER2
