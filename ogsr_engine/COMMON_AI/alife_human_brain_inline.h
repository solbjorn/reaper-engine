////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain_inline.h
//	Created 	: 06.10.2005
//  Modified 	: 06.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human brain class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CALifeHumanBrain::object_type& CALifeHumanBrain::object() const { return *XR_ASSERT_VAL(m_object != nullptr); }
IC CALifeHumanBrain::object_handler_type& CALifeHumanBrain::objects() const { return *XR_ASSERT_VAL(m_object_handler != nullptr); }
