////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_collection_storage.cpp
//	Created 	: 13.10.2005
//  Modified 	: 13.10.2005
//	Author		: Dmitriy Iassenev
//	Description : sound collection storage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sound_collection_storage.h"
#include "object_broker.h"

CSoundCollectionStorage* g_sound_collection_storage = 0;

class collection_predicate
{
private:
    typedef CSoundCollectionStorage::CSoundCollectionParams CSoundCollectionParams;
    typedef CSoundCollectionStorage::SOUND_COLLECTION_PAIR SOUND_COLLECTION_PAIR;

private:
    const CSoundCollectionParams* m_params;

public:
    IC collection_predicate(const CSoundCollectionParams& params) { m_params = &params; }

    IC bool operator()(const SOUND_COLLECTION_PAIR& pair) const { return (*m_params == pair.first); }
};

CSoundCollectionStorage::~CSoundCollectionStorage() { delete_data(m_objects); }

const CSoundCollectionStorage::SOUND_COLLECTION_PAIR& CSoundCollectionStorage::object(const CSoundCollectionParams& params)
{
    OBJECTS::const_iterator I = std::find_if(m_objects.begin(), m_objects.end(), collection_predicate(params));
    if (I != m_objects.end())
        return (*I);

    return m_objects.emplace_back(params, xr_new<CSoundCollection>(params));
}
