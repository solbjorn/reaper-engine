////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_export.cpp
//	Created 	: 01.04.2004
//  Modified 	: 22.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define SCRIPT_REGISTRATOR
#include "script_export_space.h"
#include "script_engine_export.h"

template <typename List>
struct ts_converter;

template <typename... Ts>
struct ts_converter<imdexlib::typelist<Ts...>>
{
    using handler_type = void (*)(sol::state_view& lua);

    static constexpr handler_type arr[] = {(&Ts::script_register)...};
#ifdef DEBUG
    static constexpr const char* const names[] = {(RTTI::TypeName<Ts>().data())...};
#endif

    static void script_register(sol::state_view& lua)
    {
#ifdef DEBUG
        size_t i = 0;
#endif
        for (auto handler : arr)
        {
#ifdef DEBUG
            Msg("Exporting [%s]", names[i++]);
#endif
            handler(lua);
        }
    }
};

void export_classes(sol::state_view& lua) { ts_converter<imdexlib::ts_reverse_t<script_type_list>>::script_register(lua); }
