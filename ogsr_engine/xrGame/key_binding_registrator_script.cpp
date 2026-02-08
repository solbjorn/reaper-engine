#include "stdafx.h"

#include "key_binding_registrator.h"

#include "xr_level_controller.h"

#include "../xr_3da/xr_input.h"

namespace
{
[[nodiscard]] gsl::czstring key_desc(xr::key_id key)
{
    const auto kb = dik_to_ptr(key, true);

    return kb ? kb->key_local_name.c_str() : nullptr;
}

[[nodiscard]] bool key_state(xr::key_id key) { return pInput->iGetAsyncKeyState(key); }
} // namespace

void key_binding_registrator::script_register(sol::state_view& lua)
{
    lua.new_usertype<xr::key_id>("key_id", sol::no_constructor, "binded", &get_binded_action, "desc", &key_desc, "name", &dik_to_keyname, "state", &key_state,
                                 sol::meta_function::to_string, &dik_to_keyname);
    lua.set("dik_to_bind", &get_binded_action, "bind_to_dik", &get_action_dik, "dik_to_keyname", &dik_to_keyname, "keyname_to_dik", &keyname_to_dik, "key_state", &key_state);

    auto kb = lua.create_table(actions.size(), 0);

    for (auto [id, action] : xr::views_enumerate(std::as_const(actions)))
    {
        if (action.export_name != nullptr)
            kb.set(action.export_name, id);
    }

    xr::sol_new_enum(lua, "key_bindings", kb);

    const auto keys = xr::key_ids();
    auto ks = lua.create_table(keys.size(), 0);

    for (auto& key : keys)
    {
        ks.set(std::string_view{key.key_name} | std::views::split(std::string_view{"::"}) | std::views::join_with(std::string_view{"_"}) | std::ranges::to<xr_string>(),
               std::ref(key.dik));
    }

    xr::sol_new_enum(lua, "key_ids", ks);
}
