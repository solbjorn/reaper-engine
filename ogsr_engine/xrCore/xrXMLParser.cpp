#include "stdafx.h"

#include "xrXMLParser.h"

CXml::CXml() = default;
CXml::~CXml() { ClearInternal(); }

void CXml::ClearInternal() { m_Doc.Clear(); }

void ParseFile(LPCSTR path, CMemoryWriter& W, IReader* F, CXml* xml, LPCSTR current_xml_filename)
{
    xr_string str;

    const auto loadFile = [&](LPCSTR file_name) {
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
                    FS.file_list(fset, path, FS_ListFiles, inc_name);

                    for (FS_FileSet::iterator it = fset.begin(); it != fset.end(); it++)
                    {
                        LPCSTR file_name = it->name.c_str();

                        if (strcmp(current_xml_filename, file_name) == 0)
                            continue;

                        loadFile(file_name);
                    }
                }
                else
                {
                    loadFile(inc_name);
                }
            }
        }
        else
            W.w_string(str.c_str());
    }
}

bool CXml::Init(LPCSTR path_alias, LPCSTR path, LPCSTR _xml_filename)
{
    const shared_str fn = correct_file_name(path, _xml_filename);

    string_path str;
    sprintf(str, "%s\\%s", path, *fn);
    return Init(path_alias, str);
}

// инициализация и загрузка XML файла
bool CXml::Init(LPCSTR path, LPCSTR xml_filename)
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

    m_Doc.Parse((LPCSTR)W.pointer());
    if (m_Doc.Error())
    {
        string1024 str;
        sprintf(str, "XML file:%s value:%s errDescr:%s", m_xml_file_name, m_Doc.Value(), m_Doc.ErrorStr());
        R_ASSERT(false, str);
    }

    m_root = m_Doc.FirstChildElement();

    return true;
}

XML_NODE* CXml::NavigateToNode(XML_NODE* start_node, LPCSTR path, int index)
{
    R_ASSERT(start_node && path, "NavigateToNode failed in XML file ", m_xml_file_name);
    XML_NODE* node{};
    XML_NODE* node_parent{};
    string_path buf_str{};

    VERIFY(xr_strlen(path) < 200);
    strcpy_s(buf_str, path);

    const char seps[] = ":";
    char* token;
    int tmp = 0;

    // разбить путь на отдельные подпути
    token = strtok(buf_str, seps);

    if (token)
    {
        node = (XML_NODE*)start_node->FirstChildElement(token);

        while (tmp++ < index && node)
            node = (XML_NODE*)node->NextSiblingElement(token);
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
                node = (XML_NODE*)node_parent->FirstChildElement(token);
            }
        }
    }

    return node;
}

XML_NODE* CXml::NavigateToNode(LPCSTR path, int index) { return NavigateToNode(GetLocalRoot() ? GetLocalRoot() : GetRoot(), path, index); }

XML_NODE* CXml::NavigateToNodeWithAttribute(LPCSTR tag_name, LPCSTR attrib_name, LPCSTR attrib_value)
{
    XML_NODE* root = GetLocalRoot() ? GetLocalRoot() : GetRoot();
    const int tabsCount = GetNodesNum(root, tag_name);

    for (int i = 0; i < tabsCount; ++i)
    {
        LPCSTR result = ReadAttrib(root, tag_name, i, attrib_name, "");
        if (result && xr_strcmp(result, attrib_value) == 0)
        {
            return NavigateToNode(root, tag_name, i);
        }
    }

    return nullptr;
}

bool CXml::HasNode(LPCSTR path, int index)
{
    const XML_NODE* node = NavigateToNode(path, index);
    return !!node;
}

bool CXml::HasNodeAttribute(LPCSTR path, int index, LPCSTR attrib)
{
    XML_NODE* node = NavigateToNode(path, index);
    const LPCSTR result = ReadAttrib(node, attrib, nullptr);
    return !!result;
}

LPCSTR CXml::Read(LPCSTR path, int index, LPCSTR default_str_val)
{
    XML_NODE* node = NavigateToNode(path, index);
    LPCSTR result = Read(node, default_str_val);
    return result;
}

LPCSTR CXml::Read(XML_NODE* start_node, LPCSTR path, int index, LPCSTR default_str_val)
{
    XML_NODE* node = NavigateToNode(start_node, path, index);
    LPCSTR result = Read(node, default_str_val);
    return result;
}

LPCSTR CXml::Read(XML_NODE* node, LPCSTR default_str_val)
{
    if (!node)
        return default_str_val;
    else
    {
        node = node->FirstChild();
        if (!node)
            return default_str_val;

        const XML_TEXT* text = node->ToText();
        if (text)
            return text->Value();
        else
            return default_str_val;
    }
}

int CXml::ReadInt(XML_NODE* node, int default_int_val)
{
    LPCSTR result_str = Read(node, nullptr);
    if (!result_str)
        return default_int_val;

    return atoi(result_str);
}

int CXml::ReadInt(LPCSTR path, int index, int default_int_val)
{
    LPCSTR result_str = Read(path, index, nullptr);
    if (!result_str)
        return default_int_val;

    return atoi(result_str);
}

int CXml::ReadInt(XML_NODE* start_node, LPCSTR path, int index, int default_int_val)
{
    LPCSTR result_str = Read(start_node, path, index, nullptr);
    if (!result_str)
        return default_int_val;

    return atoi(result_str);
}

float CXml::ReadFlt(LPCSTR path, int index, float default_flt_val)
{
    LPCSTR result_str = Read(path, index, nullptr);
    if (!result_str)
        return default_flt_val;

    return (float)atof(result_str);
}

float CXml::ReadFlt(XML_NODE* start_node, LPCSTR path, int index, float default_flt_val)
{
    LPCSTR result_str = Read(start_node, path, index, nullptr);
    if (!result_str)
        return default_flt_val;

    return (float)atof(result_str);
}

float CXml::ReadFlt(XML_NODE* node, float default_flt_val)
{
    LPCSTR result_str = Read(node, nullptr);
    if (!result_str)
        return default_flt_val;

    return (float)atof(result_str);
}

LPCSTR CXml::ReadAttrib(XML_NODE* start_node, LPCSTR path, int index, LPCSTR attrib, LPCSTR default_str_val)
{
    XML_NODE* node = NavigateToNode(start_node, path, index);
    LPCSTR result = ReadAttrib(node, attrib, default_str_val);

    return result;
}

LPCSTR CXml::ReadAttrib(LPCSTR path, int index, LPCSTR attrib, LPCSTR default_str_val)
{
    XML_NODE* node = NavigateToNode(path, index);
    LPCSTR result = ReadAttrib(node, attrib, default_str_val);
    return result;
}

LPCSTR CXml::ReadAttrib(XML_NODE* node, LPCSTR attrib, LPCSTR default_str_val)
{
    if (!node)
        return default_str_val;
    else
    {
        /*
                //обязательно делаем ref_str, а то
                //не сможем запомнить строку и return вернет левый указатель
                shared_str result_str;
        */
        LPCSTR result_str{};
        // Кастаем ниже по иерархии

        const XML_ELEM* el = node->ToElement();

        if (el)
        {
            result_str = el->Attribute(attrib);
            if (result_str)
                return result_str;
            else
                return default_str_val;
        }
        else
        {
            return default_str_val;
        }
    }
}

int CXml::ReadAttribInt(XML_NODE* node, LPCSTR attrib, int default_int_val)
{
    LPCSTR result_str = ReadAttrib(node, attrib, nullptr);
    if (!result_str)
        return default_int_val;

    return atoi(result_str);
}

int CXml::ReadAttribInt(LPCSTR path, int index, LPCSTR attrib, int default_int_val)
{
    LPCSTR result_str = ReadAttrib(path, index, attrib, nullptr);
    if (!result_str)
        return default_int_val;

    return atoi(result_str);
}

int CXml::ReadAttribInt(XML_NODE* start_node, LPCSTR path, int index, LPCSTR attrib, int default_int_val)
{
    LPCSTR result_str = ReadAttrib(start_node, path, index, attrib, nullptr);
    if (!result_str)
        return default_int_val;

    return atoi(result_str);
}

float CXml::ReadAttribFlt(LPCSTR path, int index, LPCSTR attrib, float default_flt_val)
{
    LPCSTR result_str = ReadAttrib(path, index, attrib, nullptr);
    if (!result_str)
        return default_flt_val;

    return (float)atof(result_str);
}

float CXml::ReadAttribFlt(XML_NODE* start_node, LPCSTR path, int index, LPCSTR attrib, float default_flt_val)
{
    LPCSTR result_str = ReadAttrib(start_node, path, index, attrib, nullptr);
    if (!result_str)
        return default_flt_val;

    return (float)atof(result_str);
}

float CXml::ReadAttribFlt(XML_NODE* node, LPCSTR attrib, float default_flt_val)
{
    LPCSTR result_str = ReadAttrib(node, attrib, nullptr);
    if (!result_str)
        return default_flt_val;

    return (float)atof(result_str);
}

int CXml::GetNodesNum(LPCSTR path, int index, LPCSTR tag_name)
{
    XML_NODE* node{};

    XML_NODE* root = GetLocalRoot() ? GetLocalRoot() : GetRoot();
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

int CXml::GetNodesNum(XML_NODE* node, LPCSTR tag_name)
{
    if (!node)
        return 0;

    XML_NODE* el{};

    if (!tag_name)
        el = node->FirstChild();
    else
        el = (XML_NODE*)node->FirstChildElement(tag_name);

    int result = 0;

    while (el)
    {
        ++result;
        if (!tag_name)
            el = el->NextSibling();
        else
            el = (XML_NODE*)el->NextSiblingElement(tag_name);
    }

    return result;
}

// нахождение элемента по его атрибуту
XML_NODE* CXml::SearchForAttribute(LPCSTR path, int index, LPCSTR tag_name, LPCSTR attrib, LPCSTR attrib_value_pattern)
{
    XML_NODE* start_node = NavigateToNode(path, index);
    XML_NODE* result = SearchForAttribute(start_node, tag_name, attrib, attrib_value_pattern);
    return result;
}

XML_NODE* CXml::SearchForAttribute(XML_NODE* start_node, LPCSTR tag_name, LPCSTR attrib, LPCSTR attrib_value_pattern)
{
    while (start_node)
    {
        XML_ELEM* el = start_node->ToElement();
        if (el)
        {
            LPCSTR attribStr = el->Attribute(attrib);
            LPCSTR valueStr = el->Value();

            if (attribStr && 0 == xr_strcmp(attribStr, attrib_value_pattern) && valueStr && 0 == xr_strcmp(valueStr, tag_name))
            {
                return el;
            }
        }

        XML_NODE* newEl = (XML_NODE*)start_node->FirstChildElement(tag_name);
        newEl = SearchForAttribute(newEl, tag_name, attrib, attrib_value_pattern);
        if (newEl)
            return newEl;

        start_node = (XML_NODE*)start_node->NextSiblingElement(tag_name);
    }

    return nullptr;
}

#ifdef DEBUG // debug & mixed

LPCSTR CXml::CheckUniqueAttrib(XML_NODE* start_node, LPCSTR tag_name, LPCSTR attrib_name)
{
    m_AttribValues.clear();

    int tags_num = GetNodesNum(start_node, tag_name);

    for (int i = 0; i < tags_num; i++)
    {
        LPCSTR attrib = ReadAttrib(start_node, tag_name, i, attrib_name, nullptr);

        xr_vector<shared_str>::iterator it = std::find(m_AttribValues.begin(), m_AttribValues.end(), attrib);

        if (m_AttribValues.end() != it)
            return attrib;

        m_AttribValues.push_back(attrib);
    }

    return nullptr;
}
#endif
