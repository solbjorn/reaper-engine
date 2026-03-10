#pragma once

#include <pugixml.hpp>

constexpr inline LPCSTR GAMEDATA_PATH = "$game_data$";
constexpr inline LPCSTR CONFIG_PATH = "$game_config$";
constexpr inline LPCSTR UI_PATH = "ui";
constexpr inline LPCSTR GAME_PATH = "gameplay";
constexpr inline LPCSTR STRING_TABLE_PATH = "text";

class CXml : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CXml);

public:
    string_path m_xml_file_name{};

    CXml();
    ~CXml() override;

    [[nodiscard]] bool Init(gsl::czstring path_alias, gsl::czstring path, gsl::czstring xml_filename);
    [[nodiscard]] bool Init(gsl::czstring path_alias, gsl::czstring xml_filename);

    // чтение элементов
    [[nodiscard]] gsl::czstring Read(gsl::czstring path, gsl::index index, gsl::czstring default_str_val) const;
    [[nodiscard]] gsl::czstring Read(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring default_str_val) const;
    [[nodiscard]] gsl::czstring Read(pugi::xml_node node, gsl::czstring default_str_val) const;

    [[nodiscard]] s32 ReadInt(gsl::czstring path, gsl::index index, s32 default_int_val) const;
    [[nodiscard]] s32 ReadInt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, s32 default_int_val) const;
    [[nodiscard]] s32 ReadInt(pugi::xml_node node, s32 default_int_val) const;

    [[nodiscard]] f32 ReadFlt(gsl::czstring path, gsl::index index, f32 default_flt_val) const;
    [[nodiscard]] f32 ReadFlt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, f32 default_flt_val) const;
    [[nodiscard]] f32 ReadFlt(pugi::xml_node node, f32 default_flt_val) const;

    [[nodiscard]] gsl::czstring ReadAttrib(gsl::czstring path, gsl::index index, gsl::czstring attrib, gsl::czstring default_str_val = "") const;
    [[nodiscard]] gsl::czstring ReadAttrib(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib, gsl::czstring default_str_val = "") const;
    [[nodiscard]] gsl::czstring ReadAttrib(pugi::xml_node node, gsl::czstring attrib, gsl::czstring default_str_val) const;

    [[nodiscard]] s32 ReadAttribInt(gsl::czstring path, gsl::index index, gsl::czstring attrib, s32 default_int_val = 0) const;
    [[nodiscard]] s32 ReadAttribInt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib, s32 default_int_val = 0) const;
    [[nodiscard]] s32 ReadAttribInt(pugi::xml_node node, gsl::czstring attrib, s32 default_int_val) const;

    [[nodiscard]] f32 ReadAttribFlt(gsl::czstring path, gsl::index index, gsl::czstring attrib, f32 default_flt_val = 0.0f) const;
    [[nodiscard]] f32 ReadAttribFlt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib, f32 default_flt_val = 0.0f) const;
    [[nodiscard]] f32 ReadAttribFlt(pugi::xml_node node, gsl::czstring attrib, f32 default_flt_val = 0.0f) const;

    [[nodiscard]] pugi::xml_node SearchForAttribute(gsl::czstring path, gsl::index index, gsl::czstring tag_name, gsl::czstring attrib, gsl::czstring attrib_value_pattern) const;
    [[nodiscard]] pugi::xml_node SearchForAttribute(pugi::xml_node start_node, gsl::czstring tag_name, gsl::czstring attrib, gsl::czstring attrib_value_pattern) const;

    // возвращает количество узлов с заданым именем
    [[nodiscard]] gsl::index GetNodesNum(gsl::czstring path, gsl::index index, gsl::czstring tag_name) const;
    [[nodiscard]] gsl::index GetNodesNum(pugi::xml_node node, gsl::czstring tag_name) const;

#ifdef DEBUG // debug & mixed
    // проверка того, что аттрибуты у тегов уникальны
    //(если не NULL, то уникальность нарушена и возврашается имя
    // повторяющегося атрибута)
    [[nodiscard]] gsl::czstring CheckUniqueAttrib(pugi::xml_node start_node, gsl::czstring tag_name, gsl::czstring attrib_name);
#endif

    // переместиться по XML дереву
    // путь задается в форме PARENT:CHILD:CHIDLS_CHILD
    //  node_index - номер, если узлов с одним именем несколько
    [[nodiscard]] pugi::xml_node NavigateToNode(gsl::czstring path, gsl::index index = 0) const;
    [[nodiscard]] pugi::xml_node NavigateToNode(pugi::xml_node start_node, gsl::czstring path, gsl::index index = 0) const;
    [[nodiscard]] pugi::xml_node NavigateToNodeWithAttribute(gsl::czstring tag_name, gsl::czstring attrib_name, gsl::czstring attrib_value) const;
    [[nodiscard]] bool HasNode(gsl::czstring path, gsl::index index) const;
    [[nodiscard]] bool HasNodeAttribute(gsl::czstring path, gsl::index index, gsl::czstring attrib) const;

    void SetLocalRoot(pugi::xml_node pLocalRoot) { m_pLocalRoot = pLocalRoot; }
    [[nodiscard]] pugi::xml_node GetLocalRoot() const { return m_pLocalRoot; }

    [[nodiscard]] pugi::xml_node GetRoot() const { return m_root; }

protected:
    pugi::xml_node m_root;
    pugi::xml_node m_pLocalRoot;

#ifdef DEBUG // debug & mixed
    // буфферный вектор для проверки уникальность аттрибутов
    xr_vector<shared_str> m_AttribValues;
#endif

public:
    [[nodiscard]] virtual shared_str correct_file_name(gsl::czstring, gsl::czstring fn) const { return shared_str{fn}; }

private:
    pugi::xml_document m_Doc;

    [[nodiscard]] pugi::xml_text text(pugi::xml_node node) const;
    [[nodiscard]] pugi::xml_text text(gsl::czstring path, gsl::index index) const;
    [[nodiscard]] pugi::xml_text text(pugi::xml_node start_node, gsl::czstring path, gsl::index index) const;

    [[nodiscard]] pugi::xml_attribute attribute(pugi::xml_node node, gsl::czstring attrib) const;
    [[nodiscard]] pugi::xml_attribute attribute(gsl::czstring path, gsl::index index, gsl::czstring attrib) const;
    [[nodiscard]] pugi::xml_attribute attribute(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib) const;
};

namespace xr
{
namespace detail
{
void xml_init();
}
} // namespace xr
