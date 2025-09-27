//////////////////////////////////////////////////////////////////////////
// relation_registry_defs.h:	реестр для хранения данных об отношении персонажа к
//								другим персонажам
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_loader.h"
#include "object_saver.h"

// структура, описывающая отношение одного персонажа к другому или к группировке
struct SRelation
{
    SRelation();

    CHARACTER_GOODWILL Goodwill() const { return m_iGoodwill; }
    void SetGoodwill(CHARACTER_GOODWILL new_goodwill) { m_iGoodwill = new_goodwill; }

private:
    // благосклонность
    CHARACTER_GOODWILL m_iGoodwill;
};

DEFINE_MAP(u16, SRelation, PERSONAL_RELATION_MAP, PERSONAL_RELATION_MAP_IT);
DEFINE_MAP(CHARACTER_COMMUNITY_INDEX, SRelation, COMMUNITY_RELATION_MAP, COMMUNITY_RELATION_MAP_IT);

// структура, существует для каждого персонажа в игре
struct RELATION_DATA
{
    void clear();

    void load(IReader&);
    void save(IWriter&) const;

    // личные отношения
    PERSONAL_RELATION_MAP personal;
    xr_vector<u16> reverse_personal;
    // отношения с группировками
    COMMUNITY_RELATION_MAP communities;
};

template <typename M>
struct object_loader::default_load<RELATION_DATA, M>
{
    void operator()(RELATION_DATA& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<RELATION_DATA, M>
{
    void operator()(const RELATION_DATA& data, M& stream) const { data.save(stream); }
};
