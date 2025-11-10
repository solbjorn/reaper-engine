#pragma once

#include "UIStatic.h"
#include "../UIStaticItem.h"

class CIconParams
{
    bool loaded{};

public:
    bool show_condition{};
    int icon_group{};
    float grid_x{}, grid_y{}, grid_width{}, grid_height{};
    shared_str name{}, section{};

    CIconParams() = default;
    explicit CIconParams(const shared_str& section) { Load(section); }
    explicit CIconParams(gsl::czstring section) { Load(shared_str{section}); }

    void Load(const shared_str& item_section);
    [[nodiscard]] ui_shader& get_shader() const;
    [[nodiscard]] Frect& original_rect() const;
    void set_shader(CUIStatic*);
    void set_shader(CUIStaticItem*);
};
