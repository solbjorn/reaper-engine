#pragma once

#include "xrXMLParser.h"

class CUIXml : public CXml
{
    RTTI_DECLARE_TYPEINFO(CUIXml, CXml);

public:
    int m_dbg_id;

    CUIXml();
    ~CUIXml() override;

    [[nodiscard]] shared_str correct_file_name(gsl::czstring path, gsl::czstring fn) const override;
};

void dump_list_xmls();
