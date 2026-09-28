#include "stdafx.h"

#include "pure_relcase.h"

#include "xr_object_list.h"
#include "IGame_Level.h"

pure_relcase::pure_relcase(CObjectList::RELCASE_CALLBACK cb) { XR_ASSERT_VAL(g_pGameLevel != nullptr)->Objects.relcase_register(cb, &m_ID); }

pure_relcase::~pure_relcase()
{
    if (g_pGameLevel != nullptr)
        g_pGameLevel->Objects.relcase_unregister(&m_ID);
}
