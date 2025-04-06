////////////////////////////////////////////////////////////////////////////
//	Module 		: luabind_memory.h
//	Created 	: 24.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : luabind memory
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <excpt.h>

namespace luabind {
	typedef void* memory_allocation_function_parameter;
	typedef void* (__cdecl* memory_allocation_function_pointer) (memory_allocation_function_parameter parameter, void const*, size_t);

	typedef void (__cdecl* log_function_pointer) (const char* format, ...);
	typedef long (* exception_filter_function_pointer) (const char* header, _EXCEPTION_POINTERS* pExceptionInfo);

	extern LUABIND_API	memory_allocation_function_pointer		allocator;
	extern LUABIND_API	memory_allocation_function_parameter	allocator_parameter;

	extern LUABIND_API	log_function_pointer			log;
	extern LUABIND_API	exception_filter_function_pointer	exception_filter;

	inline void* call_allocator	(void const* buffer, size_t const size)
	{
		return			(allocator(allocator_parameter, buffer, size));
	}

#define call_log(fmt, ...)	log(fmt, ##__VA_ARGS__)
#define call_exception(str)	exception_filter(str, GetExceptionInformation())
} // namespace luabind

#include <luabind/luabind_types.h>
#include <luabind/luabind_delete.h>
#include <luabind/luabind_memory_manager_generator.h>
