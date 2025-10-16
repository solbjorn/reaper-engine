////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_object_handler_inline.h
//	Created 	: 07.10.2005
//  Modified 	: 07.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human object handler class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline CALifeHumanObjectHandler::CALifeHumanObjectHandler(object_type* object) : m_object{object} { VERIFY(object); }

inline CALifeHumanObjectHandler::object_type& CALifeHumanObjectHandler::object() const
{
    VERIFY(m_object);
    return (*m_object);
}
