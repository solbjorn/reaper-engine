#ifndef __XRGAME_EMBEDDED_EDITOR_H
#define __XRGAME_EMBEDDED_EDITOR_H

namespace xr
{
class ingame_editor;

[[nodiscard]] std::unique_ptr<xr::ingame_editor> ingame_editor_create();
void ingame_editor_destroy(std::unique_ptr<xr::ingame_editor>& editor);
} // namespace xr

#endif // !__XRGAME_EMBEDDED_EDITOR_H
