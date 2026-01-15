//////////////////////////////////////////////////////////////////////////
// string_table.h:		таблица строк используемых в игре
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "string_table_defs.h"

using STRING_TABLE_MAP = string_unordered_map<STRING_ID, STRING_VALUE>;

struct STRING_TABLE_DATA
{
    shared_str m_sLanguage;

    STRING_TABLE_MAP m_StringTable;
    STRING_TABLE_MAP m_string_key_binding;
};

class CStringTable
{
public:
    static void Destroy();

    static tmc::task<void> ReloadLanguage();
    static STRING_VALUE translate(const STRING_ID& str_id);

    static void ReparseKeyBindings();
    static shared_str GetLanguage();

    static BOOL WriteErrorsToLog;

private:
    static tmc::task<void> Init();
    static tmc::task<void> Load(gsl::czstring xml_file);

    static void SetLanguage();
    static std::pair<STRING_VALUE, bool> ParseLine(gsl::czstring str, bool bFirst);

    static tmc::mutex pDataMutex;
    static STRING_TABLE_DATA* pData;
};
