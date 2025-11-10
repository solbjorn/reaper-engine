////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_abstract.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item abstract class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_factory_space.h"

class XR_NOVTABLE CObjectItemAbstract : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CObjectItemAbstract);

protected:
    CLASS_ID m_clsid;
    shared_str m_script_clsid;

public:
    inline explicit CObjectItemAbstract(CLASS_ID clsid, LPCSTR script_clsid);

    inline CLASS_ID clsid() const;
    inline shared_str script_clsid() const;

    virtual ObjectFactory::CLIENT_BASE_CLASS* client_object() const = 0;
    virtual ObjectFactory::SERVER_BASE_CLASS* server_object(LPCSTR section) const = 0;
};

#include "object_item_abstract_inline.h"
