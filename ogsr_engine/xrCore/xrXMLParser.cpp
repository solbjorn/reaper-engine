#include "stdafx.h"

#include "xrXMLParser.h"

namespace xr
{
namespace detail
{
void xml_init()
{
    pugi::set_memory_management_functions([](size_t size) { return xr_malloc(gsl::narrow_cast<gsl::index>(size)); }, [](void* ptr) { xr_free(ptr); });
}
} // namespace detail
} // namespace xr

namespace
{
void ParseFile(gsl::czstring path, CMemoryWriter& W, IReader* F, CXml* xml, gsl::czstring current_xml_filename)
{
    xr_string str;

    const auto loadFile = [&](gsl::czstring file_name) {
        IReader* I = nullptr;

        if (file_name == strstr(file_name, "ui\\"))
        {
            shared_str fn = xml->correct_file_name("ui", strchr(file_name, '\\') + 1);
            string_path buff;
            strconcat(sizeof(buff), buff, "ui\\", fn.c_str());
            I = FS.r_open(path, buff);
        }

        if (!I)
        {
            I = FS.r_open(path, file_name);
        }

        if (!I)
        {
            string1024 str;
            xr_sprintf(str, "XML file[%s] parsing failed. Can't find include file:[%s]", path, file_name);
            R_ASSERT(false, str);
        }

        I->skip_bom(file_name);

        ParseFile(path, W, I, xml, file_name);
        FS.r_close(I);
    };

    while (!F->eof())
    {
        F->r_string(str);

        if (str[0] && (str[0] == '#') && strstr(str.c_str(), "#include"))
        {
            if (string256 inc_name; _GetItem(str.c_str(), 1, inc_name, '"'))
            {
                if (strstr(inc_name, "*.xml"))
                {
                    FS_FileSet fset;
                    std::ignore = FS.file_list(fset, path, FS_ListFiles, inc_name);

                    for (FS_FileSet::iterator it = fset.begin(); it != fset.end(); it++)
                    {
                        if (std::is_eq(xr_strcmp(current_xml_filename, it->name)))
                            continue;

                        loadFile(it->name.c_str());
                    }
                }
                else
                {
                    loadFile(inc_name);
                }
            }
        }
        else
        {
            W.w_string(str.c_str());
        }
    }
}
} // namespace

CXml::CXml() = default;
CXml::~CXml() = default;

bool CXml::Init(gsl::czstring path_alias, gsl::czstring path, gsl::czstring _xml_filename)
{
    const shared_str fn = correct_file_name(path, _xml_filename);

    string_path str;
    sprintf(str, "%s\\%s", path, *fn);

    return Init(path_alias, str);
}

// инициализация и загрузка XML файла
bool CXml::Init(gsl::czstring path, gsl::czstring xml_filename)
{
    strcpy_s(m_xml_file_name, xml_filename);
    // Load and parse xml file

    IReader* F = FS.r_open(path, xml_filename);
    if (!F)
        return false;

    F->skip_bom(xml_filename);

    CMemoryWriter W;
    ParseFile(path, W, F, this, xml_filename);
    W.w_stringZ("");
    FS.r_close(F);

    const auto ret = m_Doc.load_string(reinterpret_cast<gsl::czstring>(W.pointer()));
    if (!ret)
        FATAL("XML file: %s, value: -%d, errDescr: %s at %zd bytes", m_xml_file_name, ret.status, ret.description(), ret.offset);

    m_root = m_Doc.first_child();

    return true;
}

pugi::xml_node CXml::NavigateToNode(pugi::xml_node start_node, gsl::czstring path, gsl::index index) const
{
    R_ASSERT(start_node && path, "NavigateToNode failed in XML file ", m_xml_file_name);
    pugi::xml_node node;
    pugi::xml_node node_parent;
    string_path buf_str{};

    VERIFY(xr_strlen(path) < 200);
    strcpy_s(buf_str, path);

    const char seps[] = ":";
    char* token;
    gsl::index tmp{0};

    // разбить путь на отдельные подпути
    token = strtok(buf_str, seps);

    if (token)
    {
        node = start_node.child(token);

        while (tmp++ < index && node)
            node = node.next_sibling(token);
    }

    while (token)
    {
        // Get next token:
        token = strtok(nullptr, seps);
        if (token)
        {
            if (node)
            {
                node_parent = node;
                node = node_parent.child(token);
            }
        }
    }

    return node;
}

pugi::xml_node CXml::NavigateToNode(gsl::czstring path, gsl::index index) const { return NavigateToNode(GetLocalRoot() ? GetLocalRoot() : GetRoot(), path, index); }

pugi::xml_node CXml::NavigateToNodeWithAttribute(gsl::czstring tag_name, gsl::czstring attrib_name, gsl::czstring attrib_value) const
{
    pugi::xml_node root = GetLocalRoot() ? GetLocalRoot() : GetRoot();
    const auto tabsCount = GetNodesNum(root, tag_name);

    for (gsl::index i{0}; i < tabsCount; ++i)
    {
        gsl::czstring result = ReadAttrib(root, tag_name, i, attrib_name, nullptr);
        if (result != nullptr && std::is_eq(xr_strcmp(result, attrib_value)))
            return NavigateToNode(root, tag_name, i);
    }

    return pugi::xml_node{};
}

bool CXml::HasNode(gsl::czstring path, gsl::index index) const { return !!NavigateToNode(path, index); }
bool CXml::HasNodeAttribute(gsl::czstring path, gsl::index index, gsl::czstring attrib) const { return !!attribute(NavigateToNode(path, index), attrib); }

pugi::xml_text CXml::text(gsl::czstring path, gsl::index index) const { return text(NavigateToNode(path, index)); }
gsl::czstring CXml::Read(gsl::czstring path, gsl::index index, gsl::czstring default_str_val) const { return text(path, index).as_string(default_str_val); }

pugi::xml_text CXml::text(pugi::xml_node start_node, gsl::czstring path, gsl::index index) const { return text(NavigateToNode(start_node, path, index)); }

gsl::czstring CXml::Read(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring default_str_val) const
{
    return text(start_node, path, index).as_string(default_str_val);
}

pugi::xml_text CXml::text(pugi::xml_node node) const { return node.text(); }
gsl::czstring CXml::Read(pugi::xml_node node, gsl::czstring default_str_val) const { return text(node).as_string(default_str_val); }

s32 CXml::ReadInt(pugi::xml_node node, s32 default_int_val) const { return text(node).as_int(default_int_val); }
s32 CXml::ReadInt(gsl::czstring path, gsl::index index, s32 default_int_val) const { return text(path, index).as_int(default_int_val); }
s32 CXml::ReadInt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, s32 default_int_val) const { return text(start_node, path, index).as_int(default_int_val); }

f32 CXml::ReadFlt(gsl::czstring path, gsl::index index, f32 default_flt_val) const { return text(path, index).as_float(default_flt_val); }
f32 CXml::ReadFlt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, f32 default_flt_val) const { return text(start_node, path, index).as_float(default_flt_val); }
f32 CXml::ReadFlt(pugi::xml_node node, f32 default_flt_val) const { return text(node).as_float(default_flt_val); }

pugi::xml_attribute CXml::attribute(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib) const
{
    return NavigateToNode(start_node, path, index).attribute(attrib);
}

gsl::czstring CXml::ReadAttrib(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib, gsl::czstring default_str_val) const
{
    return attribute(start_node, path, index, attrib).as_string(default_str_val);
}

pugi::xml_attribute CXml::attribute(gsl::czstring path, gsl::index index, gsl::czstring attrib) const { return NavigateToNode(path, index).attribute(attrib); }

gsl::czstring CXml::ReadAttrib(gsl::czstring path, gsl::index index, gsl::czstring attrib, gsl::czstring default_str_val) const
{
    return attribute(path, index, attrib).as_string(default_str_val);
}

pugi::xml_attribute CXml::attribute(pugi::xml_node node, gsl::czstring attrib) const { return node.attribute(attrib); }
gsl::czstring CXml::ReadAttrib(pugi::xml_node node, gsl::czstring attrib, gsl::czstring default_str_val) const { return attribute(node, attrib).as_string(default_str_val); }

s32 CXml::ReadAttribInt(pugi::xml_node node, gsl::czstring attrib, s32 default_int_val) const { return attribute(node, attrib).as_int(default_int_val); }
s32 CXml::ReadAttribInt(gsl::czstring path, gsl::index index, gsl::czstring attrib, s32 default_int_val) const { return attribute(path, index, attrib).as_int(default_int_val); }

s32 CXml::ReadAttribInt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib, s32 default_int_val) const
{
    return attribute(start_node, path, index, attrib).as_int(default_int_val);
}

f32 CXml::ReadAttribFlt(gsl::czstring path, gsl::index index, gsl::czstring attrib, f32 default_flt_val) const { return attribute(path, index, attrib).as_float(default_flt_val); }

f32 CXml::ReadAttribFlt(pugi::xml_node start_node, gsl::czstring path, gsl::index index, gsl::czstring attrib, f32 default_flt_val) const
{
    return attribute(start_node, path, index, attrib).as_float(default_flt_val);
}

f32 CXml::ReadAttribFlt(pugi::xml_node node, gsl::czstring attrib, f32 default_flt_val) const { return attribute(node, attrib).as_float(default_flt_val); }

gsl::index CXml::GetNodesNum(gsl::czstring path, gsl::index index, gsl::czstring tag_name) const
{
    pugi::xml_node root = GetLocalRoot() ? GetLocalRoot() : GetRoot();
    pugi::xml_node node;

    if (path)
    {
        node = NavigateToNode(path, index);
        if (!node)
            node = root;
    }
    else
    {
        node = root;
    }

    if (!node)
        return 0;

    return GetNodesNum(node, tag_name);
}

gsl::index CXml::GetNodesNum(pugi::xml_node node, gsl::czstring tag_name) const
{
    if (!node)
        return 0;

    pugi::xml_node el = tag_name != nullptr ? node.child(tag_name) : node.first_child();
    gsl::index result{0};

    while (el)
    {
        el = tag_name != nullptr ? el.next_sibling(tag_name) : el.next_sibling();
        ++result;
    }

    return result;
}

// нахождение элемента по его атрибуту
pugi::xml_node CXml::SearchForAttribute(gsl::czstring path, gsl::index index, gsl::czstring tag_name, gsl::czstring attrib, gsl::czstring attrib_value_pattern) const
{
    return SearchForAttribute(NavigateToNode(path, index), tag_name, attrib, attrib_value_pattern);
}

pugi::xml_node CXml::SearchForAttribute(pugi::xml_node start_node, gsl::czstring tag_name, gsl::czstring attrib, gsl::czstring attrib_value_pattern) const
{
    while (start_node)
    {
        std::string_view attribStr{start_node.attribute(attrib).value()};
        std::string_view valueStr{start_node.value()};

        if (std::is_eq(xr_strcmp(attribStr, attrib_value_pattern)) && std::is_eq(xr_strcmp(valueStr, tag_name)))
            return start_node;

        pugi::xml_node newEl = SearchForAttribute(start_node.child(tag_name), tag_name, attrib, attrib_value_pattern);
        if (newEl)
            return newEl;

        start_node = start_node.next_sibling(tag_name);
    }

    return pugi::xml_node{};
}

#ifdef DEBUG // debug & mixed
gsl::czstring CXml::CheckUniqueAttrib(pugi::xml_node start_node, gsl::czstring tag_name, gsl::czstring attrib_name)
{
    m_AttribValues.clear();

    const auto tags_num = GetNodesNum(start_node, tag_name);

    for (gsl::index i{0}; i < tags_num; i++)
    {
        gsl::czstring attrib = ReadAttrib(start_node, tag_name, i, attrib_name, nullptr);

        xr_vector<shared_str>::iterator it = std::find(m_AttribValues.begin(), m_AttribValues.end(), attrib);

        if (m_AttribValues.end() != it)
            return attrib;

        m_AttribValues.push_back(attrib);
    }

    return nullptr;
}
#endif
