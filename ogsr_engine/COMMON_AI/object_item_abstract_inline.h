////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_abstract_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item abstract class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef object_item_abstract_inlineH
#define object_item_abstract_inlineH

inline CObjectItemAbstract::CObjectItemAbstract(CLASS_ID clsid, LPCSTR script_clsid) : m_clsid{clsid}, m_script_clsid{script_clsid} {}

inline CLASS_ID CObjectItemAbstract::clsid() const { return m_clsid; }
inline shared_str CObjectItemAbstract::script_clsid() const { return m_script_clsid; }

#endif
