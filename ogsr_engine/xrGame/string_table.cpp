#include "stdafx.h"

#include "string_table.h"

#include "ui/xrUIXmlParser.h"
#include "xr_level_controller.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wunused-template");

#include <oneapi/tbb/parallel_for_each.h>

XR_DIAG_POP();

std::mutex CStringTable::pDataMutex;
STRING_TABLE_DATA* CStringTable::pData{};

BOOL CStringTable::WriteErrorsToLog{};

CStringTable::CStringTable() { Init(); }

void CStringTable::Destroy() { xr_delete(pData); }

shared_str CStringTable::GetLanguage() { return pData->m_sLanguage; }

void CStringTable::Init()
{
    if (pData)
        return;

    pData = xr_new<STRING_TABLE_DATA>();

    // имя языка, если не задано (NULL), то первый <text> в <string> в XML
    pData->m_sLanguage._set(pSettings->r_string("string_table", "language"));

    LPCSTR S = pSettings->r_string("string_table", "files");
    if (S && S[0])
    {
        size_t count = _GetItemCount(S);

        oneapi::tbb::parallel_for(oneapi::tbb::blocked_range<size_t>(0, count), [this, &S](const auto& range) {
            for (size_t i = range.begin(); i != range.end(); i++)
            {
                string128 xml_file;

                std::ignore = _GetItem(S, i, xml_file);
                Load(xml_file);
            }
        });
    }

    if (pSettings->section_exist("string_table_files"))
    {
        CInifile::Sect& files = pSettings->r_section("string_table_files");
        oneapi::tbb::parallel_for_each(files.Ordered_Data, [&](const auto& i) { Load(i.first.c_str()); });
    }
}

void CStringTable::Load(LPCSTR xml_file)
{
    const char* lang = *pData->m_sLanguage;
    string_path xml_file_full, _s;
    CUIXml uiXml;
    bool nf;

    strconcat(sizeof(xml_file_full), xml_file_full, xml_file, ".xml");

    if (FS.exist(_s, "$game_config$", "text\\", xml_file_full))
    {
        strcpy_s(_s, STRING_TABLE_PATH);
        nf = true;
    }
    else
    {
        strconcat(sizeof(_s), _s, STRING_TABLE_PATH, "\\", lang);
        nf = false;
    }

    bool xml_result = uiXml.Init(CONFIG_PATH, _s, xml_file_full);
    if (!xml_result)
        Debug.fatal(DEBUG_INFO, "string table xml file not found %s, for language %s", xml_file_full, lang);

    // общий список всех записей таблицы в файле
    const size_t string_num = uiXml.GetNodesNum(uiXml.GetRoot(), "string");

    for (size_t i = 0; i < string_num; i++)
    {
        LPCSTR string_name = uiXml.ReadAttrib(uiXml.GetRoot(), "string", i, "id", nullptr);

        string32 node;
        strconcat(sizeof(node), node, "string:", nf ? lang : "text");

        LPCSTR string_text = uiXml.Read(uiXml.GetRoot(), node, i, nullptr);
        ASSERT_FMT(string_text, "no attribute '%s' in node %s", node, string_name);

        const auto str_val{ParseLine(string_text, string_name, true)};
        std::scoped_lock lock{pDataMutex};

        if (WriteErrorsToLog && pData->m_StringTable.contains(string_name))
            Msg("!![%s] duplicate string table id: [%s]", __FUNCTION__, string_name);

        pData->m_StringTable[string_name] = str_val;
    }
}

void CStringTable::ReparseKeyBindings()
{
    if (!pData)
        return;

    for (const auto& [key, val] : pData->m_string_key_binding)
    {
        const auto str{ParseLine(val.c_str(), key.c_str(), false)};
        std::scoped_lock lock{pDataMutex};
        pData->m_StringTable[key] = str;
    }
}

STRING_VALUE CStringTable::ParseLine(LPCSTR str, LPCSTR skey, bool bFirst)
{
    //	LPCSTR str = "1 $$action_left$$ 2 $$action_right$$ 3 $$action_left$$ 4";
    xr_string res;
    gsl::index k{};
    const char* b;

#define ACTION_STR "$$ACTION_"
#define LEN 9

    string256 buff;
    string256 srcbuff;
    bool b_hit = false;

    while ((b = strstr(str + k, ACTION_STR)) != nullptr)
    {
        buff[0] = 0;
        srcbuff[0] = 0;
        res.append(str + k, b - str - k);
        const char* e = strstr(b + LEN, "$$");

        int len = (int)(e - b - LEN);

        strncpy_s(srcbuff, b + LEN, len);
        srcbuff[len] = 0;
        GetActionAllBinding(srcbuff, buff, sizeof(buff));
        res.append(buff, xr_strlen(buff));

        k = b - str;
        k += len;
        k += LEN;
        k += 2;

        b_hit = true;
    }

    if (k < xr_strlen(str))
        res.append(str + k);

    if (b_hit && bFirst)
    {
        std::scoped_lock lock{pDataMutex};
        pData->m_string_key_binding[skey]._set(str);
    }

    return STRING_VALUE(res.c_str());
}

STRING_VALUE CStringTable::translate(const STRING_ID& str_id) const
{
    STRING_VALUE res = str_id;

    if (!pData)
        return res;

    res = pData->m_StringTable[str_id];
    if (!res)
    {
        if (WriteErrorsToLog && *str_id != nullptr && xr_strlen(*str_id) > 0)
            Msg("!![%s] [%s] has no entry!", __FUNCTION__, *str_id);

        res = str_id;
    }

    return res;
}
