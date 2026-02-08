#pragma once

#include "../xr_3da/IInputReceiver.h"

enum class EGameActions : s32
{
    kLEFT = 0,
    kRIGHT,
    kUP,
    kDOWN,
    kJUMP,
    kCROUCH,
    kCROUCH_TOGGLE,
    kACCEL,
    kSPRINT_TOGGLE,

    kFWD,
    kBACK,
    kL_STRAFE,
    kR_STRAFE,

    kL_LOOKOUT,
    kR_LOOKOUT,

    kENGINE,
    kCAM_1,
    kCAM_2,
    kCAM_3,

    kTORCH,
    kNIGHT_VISION,
    kWPN_1,
    kWPN_2,
    kWPN_3,
    kWPN_4,
    kWPN_5,
    kWPN_6,
    kWPN_8, // kDETECTOR
    kLASER_ON,
    kWPN_NEXT,
    kWPN_FIRE,
    kWPN_ZOOM,
    kWPN_ZOOM_INC,
    kWPN_ZOOM_DEC,
    kWPN_RELOAD,
    kWPN_FUNC,
    kWPN_FIREMODE_PREV,
    kWPN_FIREMODE_NEXT,

    kPAUSE,
    kDROP,
    kUSE,
    kSCORES,
    kFLASHLIGHT,
    kSCREENSHOT,
    kQUIT,
    kCONSOLE,
    kINVENTORY,
    kACTIVE_JOBS,
    kMAP,
    kCONTACTS,
    kEXT_1,

    kNEXT_SLOT,
    kPREV_SLOT,

    kUSE_BANDAGE,
    kUSE_MEDKIT,

    kQUICK_SAVE,
    kQUICK_LOAD,

    kHIDEHUD,
    kSHOWHUD,

    kNOTBINDED = std::numeric_limits<s32>::max(),
};

struct _keyboard
{
    gsl::czstring key_name;
    xr::key_id dik;
    xr_string key_local_name;

    constexpr explicit _keyboard(gsl::czstring name, xr::key_id d) : key_name{name}, dik{d} {}
};

struct _action
{
    gsl::czstring action_name;
    gsl::czstring export_name;
};

[[nodiscard]] gsl::czstring dik_to_keyname(xr::key_id _dik);
[[nodiscard]] xr::key_id keyname_to_dik(gsl::czstring _name);
[[nodiscard]] _keyboard* dik_to_ptr(xr::key_id _dik, bool bSafe);

[[nodiscard]] EGameActions action_name_to_id(gsl::czstring _name);
[[nodiscard]] _action* action_name_to_ptr(gsl::czstring _name);

extern xr_vector<_action> actions;

namespace xr
{
[[nodiscard]] EGameActions action_id(const _action& act);
[[nodiscard]] std::span<const _keyboard> key_ids();
} // namespace xr

struct _binding
{
    _action* m_action;
    std::array<_keyboard*, 2> m_keyboard{};

    constexpr explicit _binding(_action* act) : m_action{act} {}
};

extern xr_vector<_binding> g_key_bindings;

[[nodiscard]] bool is_binded(EGameActions action_id, xr::key_id dik);
[[nodiscard]] xr::key_id get_action_dik(EGameActions action_id);
[[nodiscard]] EGameActions get_binded_action(xr::key_id dik);

void CCC_RegisterInput();

class ConsoleBindCmds
{
public:
    xr_map<xr::key_id, shared_str> m_bindConsoleCmds;

    void bind(xr::key_id dik, gsl::czstring N);
    void unbind(xr::key_id dik);
    [[nodiscard]] bool execute(xr::key_id dik) const;
    void clear();
    void save(IWriter* F) const;
};

void GetActionAllBinding(gsl::czstring action, gsl::zstring dst_buff, gsl::index dst_buff_sz);

extern ConsoleBindCmds bindConsoleCmds;
