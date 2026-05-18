// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external
//				through IUITextureControl interface
// created:		11.05.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#pragma once

#include "ui_defs.h"

class IUISimpleTextureControl;

struct TEX_INFO
{
    shared_str file;
    Frect rect;

    [[nodiscard]] constexpr auto get_file_name() const { return file.c_str(); }
    [[nodiscard]] constexpr auto get_rect() const { return rect; }
};

class CUITextureMaster
{
public:
    static void ParseShTexInfo(LPCSTR xml_file);

    static void InitTexture(LPCSTR texture_name, IUISimpleTextureControl* tc);
    static void InitTexture(LPCSTR texture_name, const char* shader_name, IUISimpleTextureControl* tc);
    static float GetTextureHeight(LPCSTR texture_name);
    static float GetTextureWidth(LPCSTR texture_name);
    static Frect GetTextureRect(LPCSTR texture_name);
    static LPCSTR GetTextureFileName(LPCSTR texture_name);
    static TEX_INFO FindItem(LPCSTR texture_name, LPCSTR def_texture_name);
    static void WriteLog();

protected:
    static xr::string_map<shared_str, TEX_INFO> m_textures;

#ifdef DEBUG
    static u32 m_time;
#endif
};
