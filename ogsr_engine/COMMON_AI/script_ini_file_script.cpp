////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file_script.cpp
//	Created 	: 25.06.2004
//  Modified 	: 25.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_ini_file.h"

namespace
{
std::tuple<bool, xr_string, xr_string> r_line(CScriptIniFile* self, LPCSTR S, int L)
{
    THROW3(self->section_exist(S), "Cannot find section", S);
    THROW2((int)self->line_count(S) > L, "Invalid line number");

    LPCSTR n{}, v{};
    bool result = !!self->r_line(S, L, &n, &v);

    return std::tuple<bool, xr_string, xr_string>(result, n ?: "", v ?: "");
}

void iterate_sections(CScriptIniFile& self, sol::function func)
{
    for (const auto& it : self.sections())
        func(it.first.c_str());
}

CScriptIniFile* reload_system_ini()
{
    pSettings->Destroy(const_cast<CInifile*>(pSettings));

    CInifile* tmp{};
    string_path fname;

    FS.update_path(fname, "$game_config$", "system_mods.ltx");
    if (FS.exist(fname))
    {
        tmp = xr_new<CInifile>(fname, TRUE, FALSE);

        tmp->load_file(TRUE);

        Msg("~ Apply system_mods.ltx...");
    }

    FS.update_path(fname, "$game_config$", "system.ltx");
    pSettings = xr_new<CInifile>(fname, TRUE, FALSE);
    pSettings->load_file(FALSE, tmp);

    CHECK_OR_EXIT(!pSettings->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

    if (tmp)
    {
        xr_delete(tmp);
    }

    return ((CScriptIniFile*)pSettings);
}
} // namespace

void CScriptIniFile::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptIniFile>(
        "ini_file", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptIniFile(LPCSTR), CScriptIniFile(LPCSTR, bool)>(), "section_exist",
        &CScriptIniFile::section_exist_script, "line_exist", &CScriptIniFile::line_exist, "line_count", &CScriptIniFile::line_count, "remove_line", &CScriptIniFile::remove_line,
        "remove_section", &CScriptIniFile::remove_section, "get_as_string", &CScriptIniFile::get_as_string, "name", &CScriptIniFile::name, "save", &CScriptIniFile::save_as,
        "iterate_sections", &iterate_sections, "readonly", &CScriptIniFile::bReadOnly, "r_line", &::r_line, "r_bool", &CScriptIniFile::r_bool, "r_string",
        &CScriptIniFile::r_string, "r_u32", &CScriptIniFile::r_u32, "r_s32", &CScriptIniFile::r_s32, "r_float", &CScriptIniFile::r_float, "r_vector2", &CScriptIniFile::r_fvector2,
        "r_vector", &CScriptIniFile::r_fvector3, "r_vector4", &CScriptIniFile::r_fvector4, "r_clsid", &CScriptIniFile::r_clsid, "r_string_wq", &CScriptIniFile::r_string_wb,
        "w_bool", &CScriptIniFile::w_bool, "w_string", &CScriptIniFile::w_string, "w_u32", &CScriptIniFile::w_u32, "w_s32", &CScriptIniFile::w_s32, "w_float",
        &CScriptIniFile::w_float, "w_vector2", &CScriptIniFile::w_fvector2, "w_vector", &CScriptIniFile::w_fvector3, "w_vector4", &CScriptIniFile::w_fvector4);

    lua.set("system_ini", [] { return reinterpret_cast<CScriptIniFile*>(pSettings); }, "game_ini", [] { return reinterpret_cast<CScriptIniFile*>(pGameIni); });

    // чтение ini как текста, без возможности сохранить
    lua.set_function("create_ini_file", [](const char* ini_string) {
        IReader reader{(void*)ini_string, gsl::narrow_cast<size_t>(xr_strlen(ini_string))};
        return std::make_unique<CScriptIniFile>(&reader, FS.get_path("$game_config$")->m_Path);
    });

    lua.set_function("reload_system_ini", &reload_system_ini);
}
