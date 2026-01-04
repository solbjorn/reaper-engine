#include "stdafx.h"

#include "embedded_editor.h"

#include "embedded_editor_main.h"

#include "../xrExternal/imgui.h"

namespace xr
{
#ifndef IMGUI_DISABLE
std::unique_ptr<xr::ingame_editor> ingame_editor_create()
{
    auto real = xr_new<xr::detail::imgui_ingame_editor>();
    return absl::WrapUnique(static_cast<xr::ingame_editor*>(real));
}

void ingame_editor_destroy(std::unique_ptr<xr::ingame_editor>& editor)
{
    auto real = smart_cast<xr::detail::imgui_ingame_editor*>(editor.release());
    xr_free(real);
}
#else // IMGUI_DISABLE
std::unique_ptr<xr::ingame_editor> ingame_editor_create() { return std::unique_ptr<xr::ingame_editor>{}; }
void ingame_editor_destroy(std::unique_ptr<xr::ingame_editor>& editor) { editor.reset(); }
#endif // IMGUI_DISABLE
} // namespace xr
