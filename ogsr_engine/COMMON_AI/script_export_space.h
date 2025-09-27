////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_space.h
//	Created 	: 22.09.2003
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export space
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef SCRIPT_REGISTRATOR

#include <typelist.hpp>

#define script_type_list imdexlib::typelist<>
#define add_to_type_list(type) using TypeList_##type = imdexlib::ts_prepend_t<type, script_type_list>
#define save_type_list(type) TypeList_##type

#else

#define script_type_list
#define add_to_type_list(type) XR_MACRO_END()
#define save_type_list(type)

#endif

#define DECLARE_SCRIPT_REGISTER_FUNCTION() \
public: \
    static void script_register(sol::state_view& lua)

template <typename T>
struct enum_exporter
{
    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

template <typename T>
struct class_exporter
{
    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

template <typename T>
inline std::unique_ptr<DLL_Pure> client_factory(std::unique_ptr<T>& self)
{
    return std::unique_ptr<DLL_Pure>(static_cast<DLL_Pure*>(self.release()));
}

template <typename T>
inline std::unique_ptr<CSE_Abstract> server_factory(std::unique_ptr<T>& self)
{
    return std::unique_ptr<CSE_Abstract>(static_cast<CSE_Abstract*>(self.release()));
}
