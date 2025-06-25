#pragma once

#include "xrXMLParser.h"

class CUIXml : public CXml
{
    RTTI_DECLARE_TYPEINFO(CUIXml, CXml);

public:
    int m_dbg_id;

    CUIXml();
    virtual ~CUIXml();

    virtual shared_str correct_file_name(LPCSTR path, LPCSTR fn);
};
