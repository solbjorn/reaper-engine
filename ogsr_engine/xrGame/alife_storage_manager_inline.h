////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_storage_manager_inline.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator storage manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline CALifeStorageManager::CALifeStorageManager(LPCSTR section) : m_section{section} {}

IC LPCSTR CALifeStorageManager::save_name(BOOL bLoaded)
{
    if (!xr_strlen(m_loaded_save))
        bLoaded = FALSE;
    return bLoaded ? m_loaded_save : m_save_name;
}
