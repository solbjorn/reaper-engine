////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_single.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client or server class
////////////////////////////////////////////////////////////////////////////

#ifndef object_item_singleH
#define object_item_singleH

#pragma once

#include "object_factory_space.h"
#include "object_item_abstract.h"

template <typename _unknown_type, bool _client_type>
class CObjectItemSingle : public CObjectItemAbstract
{
    RTTI_DECLARE_TYPEINFO(CObjectItemSingle<_unknown_type, _client_type>, CObjectItemAbstract);

protected:
    typedef CObjectItemAbstract inherited;
    typedef _unknown_type SERVER_TYPE;

public:
    IC CObjectItemSingle(const CLASS_ID& clsid, LPCSTR script_clsid);
    virtual ObjectFactory::CLIENT_BASE_CLASS* client_object() const;
    virtual ObjectFactory::SERVER_BASE_CLASS* server_object(LPCSTR section) const;
};

template <typename _unknown_type>
class CObjectItemSingle<_unknown_type, true> : public CObjectItemAbstract
{
    RTTI_DECLARE_TYPEINFO(CObjectItemSingle<_unknown_type, true>, CObjectItemAbstract);

protected:
    typedef CObjectItemAbstract inherited;
    typedef _unknown_type CLIENT_TYPE;

public:
    IC CObjectItemSingle(const CLASS_ID& clsid, LPCSTR script_clsid);
    virtual ObjectFactory::CLIENT_BASE_CLASS* client_object() const;
    virtual ObjectFactory::SERVER_BASE_CLASS* server_object(LPCSTR section) const;
};

#include "object_item_single_inline.h"

#endif
