#pragma once

#include "../xrGame/ui/xrUIXmlParser.h"
#ifndef XRGAME_EXPORTS
#include "object_broker.h"
#endif // XRGAME_EXPORTS

// T_INIT -  класс где определена статическая InitXmlIdToIndex
//           функция инициализации file_str и tag_name

// структура хранит строковый id элемента
// файл и позицию, где этот элемент находится
struct ITEM_DATA
{
    shared_str id;
    int index;
    int pos_in_file;
    CUIXml* _xml;
};
typedef xr_vector<ITEM_DATA> T_VECTOR;

void _destroy_item_data_vector_cont(T_VECTOR* vec);

#define TEMPLATE_SPECIALIZATION template <typename T_INIT>
#define CSXML_IdToIndex CXML_IdToIndex<T_INIT>

TEMPLATE_SPECIALIZATION
class CXML_IdToIndex : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CXML_IdToIndex<T_INIT>);

private:
    static T_VECTOR* m_pItemDataVector;

protected:
    // имена xml файлов (разделенных запятой) из которых
    // производить загрузку элементов
    static LPCSTR file_str;
    // имена тегов
    static LPCSTR tag_name;

public:
    CXML_IdToIndex();
    virtual ~CXML_IdToIndex();

    static void InitInternal();

    static const ITEM_DATA* GetById(const shared_str& str_id, bool no_assert = false);
    static const ITEM_DATA* GetByIndex(int index, bool no_assert = false);

    static const int IdToIndex(const shared_str& str_id, int default_index = -1, bool no_assert = false)
    {
        const ITEM_DATA* item = GetById(str_id, no_assert);
        return item ? item->index : default_index;
    }
    static const shared_str IndexToId(int index, shared_str default_id = NULL, bool no_assert = false)
    {
        const ITEM_DATA* item = GetByIndex(index, no_assert);
        return item ? item->id : default_id;
    }

    static const int GetMaxIndex() { return m_pItemDataVector->size() - 1; }

    // удаление статичекого массива
    static void DeleteIdToIndexData();
};

TEMPLATE_SPECIALIZATION
T_VECTOR* CSXML_IdToIndex::m_pItemDataVector = NULL;

TEMPLATE_SPECIALIZATION
LPCSTR CSXML_IdToIndex::file_str = NULL;
TEMPLATE_SPECIALIZATION
LPCSTR CSXML_IdToIndex::tag_name = NULL;

TEMPLATE_SPECIALIZATION
CSXML_IdToIndex::CXML_IdToIndex() {}

TEMPLATE_SPECIALIZATION
CSXML_IdToIndex::~CXML_IdToIndex() {}

TEMPLATE_SPECIALIZATION
const ITEM_DATA* CSXML_IdToIndex::GetById(const shared_str& str_id, bool no_assert)
{
    T_INIT::InitXmlIdToIndex();

    auto it = m_pItemDataVector->begin();
    for (; m_pItemDataVector->end() != it; it++)
    {
        if ((*it).id == str_id)
            break;
    }

    if (m_pItemDataVector->end() == it)
    {
        int i = 0;
        for (T_VECTOR::iterator it = m_pItemDataVector->begin(); m_pItemDataVector->end() != it; it++, i++)
            Msg("[%d]=[%s]", i, *(*it).id);

        R_ASSERT3(no_assert, "item not found, id", *str_id);
        return NULL;
    }

    return &(*it);
}

TEMPLATE_SPECIALIZATION
const ITEM_DATA* CSXML_IdToIndex::GetByIndex(int index, bool no_assert)
{
    if ((size_t)index >= m_pItemDataVector->size())
    {
        R_ASSERT3(no_assert, "item by index not found in files", file_str);
        return NULL;
    }
    return &(*m_pItemDataVector)[index];
}

TEMPLATE_SPECIALIZATION
void CSXML_IdToIndex::DeleteIdToIndexData()
{
    VERIFY(m_pItemDataVector);
    _destroy_item_data_vector_cont(m_pItemDataVector);
    xr_delete(m_pItemDataVector);
}

TEMPLATE_SPECIALIZATION
void CSXML_IdToIndex::InitInternal()
{
    VERIFY(!m_pItemDataVector);
    T_INIT::InitXmlIdToIndex();

    m_pItemDataVector = xr_new<T_VECTOR>();

    VERIFY(file_str);
    VERIFY(tag_name);

    string_path xml_file;
    int count = _GetItemCount(file_str);
    int index = 0;
    for (int it = 0; it < count; ++it)
    {
        _GetItem(file_str, it, xml_file);

        CUIXml* uiXml = xr_new<CUIXml>();
        xr_string xml_file_full;
        xml_file_full = xml_file;
        xml_file_full += ".xml";
        bool xml_result = uiXml->Init(CONFIG_PATH, GAME_PATH, xml_file_full.c_str());
        R_ASSERT3(xml_result, "error while parsing XML file", xml_file_full.c_str());

        // общий список
        int items_num = uiXml->GetNodesNum(uiXml->GetRoot(), tag_name);

        for (int i = 0; i < items_num; ++i)
        {
            LPCSTR item_name = uiXml->ReadAttrib(uiXml->GetRoot(), tag_name, i, "id", NULL);

            string256 buf;
            sprintf_s(buf, "id for item don't set, number %d in %s", i, xml_file);
            R_ASSERT2(item_name, buf);

            // проверетить ID на уникальность
            T_VECTOR::iterator t_it = m_pItemDataVector->begin();
            for (; m_pItemDataVector->end() != t_it; t_it++)
            {
                if (!xr_strcmp((*t_it).id.c_str(), item_name))
                    break;
            }

            R_ASSERT3(m_pItemDataVector->end() == t_it, "duplicate item id", item_name);

            ITEM_DATA data;
            data.id = item_name;
            data.index = index;
            data.pos_in_file = i;
            //.				data.file_name		= xml_file;
            data._xml = uiXml;
            m_pItemDataVector->push_back(data);

            index++;
        }
        if (0 == items_num)
            xr_delete(uiXml);
    }
}

#undef TEMPLATE_SPECIALIZATION
