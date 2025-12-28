#include "stdafx.h"

#include "key_binding_registrator.h"

#include "xr_level_controller.h"

#include "../xr_3da/xr_input.h"

#include <dinput.h>

namespace
{
[[nodiscard]] int dik_to_bind(int dik) { return get_binded_action(dik); }
[[nodiscard]] int bind_to_dik(EGameActions bind) { return get_action_dik(bind); }
[[nodiscard]] bool key_state(int key) { return pInput->iGetAsyncKeyState(key); }
} // namespace

void key_binding_registrator::script_register(sol::state_view& lua)
{
    lua.set("dik_to_bind", &dik_to_bind, "bind_to_dik", &bind_to_dik, "dik_to_keyname", &dik_to_keyname, "keyname_to_dik", &keyname_to_dik, "key_state", &key_state);

    auto kb = lua.create_table(actions.size(), 0);

    for (const auto& action : actions)
    {
        if (action.export_name != nullptr)
            kb.set(action.export_name, action.id);
    }

    xr::sol_new_enum(lua, "key_bindings", kb);

    lua.new_enum("DIK_keys", "DIK_ESCAPE", DIK_ESCAPE, "DIK_2", DIK_2, "DIK_4", DIK_4, "DIK_6", DIK_6, "DIK_8", DIK_8, "DIK_0", DIK_0, "DIK_EQUALS", DIK_EQUALS, "DIK_TAB", DIK_TAB,
                 "DIK_W", DIK_W, "DIK_R", DIK_R, "DIK_Y", DIK_Y, "DIK_I", DIK_I, "DIK_P", DIK_P, "DIK_RBRACKET", DIK_RBRACKET, "DIK_LCONTROL", DIK_LCONTROL, "DIK_S", DIK_S,
                 "DIK_F", DIK_F, "DIK_H", DIK_H, "DIK_K", DIK_K, "DIK_SEMICOLON", DIK_SEMICOLON, "DIK_GRAVE", DIK_GRAVE, "DIK_BACKSLASH", DIK_BACKSLASH, "DIK_X", DIK_X, "DIK_V",
                 DIK_V, "DIK_N", DIK_N, "DIK_COMMA", DIK_COMMA, "DIK_SLASH", DIK_SLASH, "DIK_MULTIPLY", DIK_MULTIPLY, "DIK_SPACE", DIK_SPACE, "DIK_F1", DIK_F1, "DIK_F3", DIK_F3,
                 "DIK_F5", DIK_F5, "DIK_F7", DIK_F7, "DIK_F9", DIK_F9, "DIK_NUMLOCK", DIK_NUMLOCK, "DIK_NUMPAD7", DIK_NUMPAD7, "DIK_NUMPAD9", DIK_NUMPAD9, "DIK_NUMPAD4",
                 DIK_NUMPAD4, "DIK_NUMPAD6", DIK_NUMPAD6, "DIK_NUMPAD1", DIK_NUMPAD1, "DIK_NUMPAD3", DIK_NUMPAD3, "DIK_DECIMAL", DIK_DECIMAL, "DIK_F12", DIK_F12, "DIK_RCONTROL",
                 DIK_RCONTROL, "DIK_DIVIDE", DIK_DIVIDE, "DIK_RMENU", DIK_RMENU, "DIK_UP", DIK_UP, "DIK_LEFT", DIK_LEFT, "DIK_END", DIK_END, "DIK_NEXT", DIK_NEXT, "DIK_DELETE",
                 DIK_DELETE, "DIK_RWIN", DIK_RWIN, "DIK_PAUSE", DIK_PAUSE, "MOUSE_2", MOUSE_2, "DIK_1", DIK_1, "DIK_3", DIK_3, "DIK_5", DIK_5, "DIK_7", DIK_7, "DIK_9", DIK_9,
                 "DIK_MINUS", DIK_MINUS, "DIK_BACK", DIK_BACK, "DIK_Q", DIK_Q, "DIK_E", DIK_E, "DIK_T", DIK_T, "DIK_U", DIK_U, "DIK_O", DIK_O, "DIK_LBRACKET", DIK_LBRACKET,
                 "DIK_RETURN", DIK_RETURN, "DIK_A", DIK_A, "DIK_D", DIK_D, "DIK_G", DIK_G, "DIK_J", DIK_J, "DIK_L", DIK_L, "DIK_APOSTROPHE", DIK_APOSTROPHE, "DIK_LSHIFT",
                 DIK_LSHIFT, "DIK_Z", DIK_Z, "DIK_C", DIK_C, "DIK_B", DIK_B, "DIK_M", DIK_M, "DIK_PERIOD", DIK_PERIOD, "DIK_RSHIFT", DIK_RSHIFT, "DIK_LMENU", DIK_LMENU,
                 "DIK_CAPITAL", DIK_CAPITAL, "DIK_F2", DIK_F2, "DIK_F4", DIK_F4, "DIK_F6", DIK_F6, "DIK_F8", DIK_F8, "DIK_F10", DIK_F10, "DIK_SCROLL", DIK_SCROLL, "DIK_NUMPAD8",
                 DIK_NUMPAD8, "DIK_SUBTRACT", DIK_SUBTRACT, "DIK_NUMPAD5", DIK_NUMPAD5, "DIK_ADD", DIK_ADD, "DIK_NUMPAD2", DIK_NUMPAD2, "DIK_NUMPAD0", DIK_NUMPAD0, "DIK_F11",
                 DIK_F11, "DIK_NUMPADENTER", DIK_NUMPADENTER, "DIK_SYSRQ", DIK_SYSRQ, "DIK_HOME", DIK_HOME, "DIK_PRIOR", DIK_PRIOR, "DIK_RIGHT", DIK_RIGHT, "DIK_DOWN", DIK_DOWN,
                 "DIK_INSERT", DIK_INSERT, "DIK_LWIN", DIK_LWIN, "DIK_APPS", DIK_APPS, "MOUSE_1", MOUSE_1, "MOUSE_2", MOUSE_2, "MOUSE_3", MOUSE_3, "MOUSE_4", MOUSE_4, "MOUSE_5",
                 MOUSE_5, "MOUSE_6", MOUSE_6, "MOUSE_7", MOUSE_7, "MOUSE_8", MOUSE_8);
}
