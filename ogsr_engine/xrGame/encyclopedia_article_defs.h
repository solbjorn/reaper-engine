#pragma once

#include "alife_space.h"
#include "object_loader.h"
#include "object_saver.h"

struct ARTICLE_DATA
{
    enum EArticleType
    {
        eEncyclopediaArticle,
        eJournalArticle,
        eTaskArticle,
        eInfoArticle
    };

    ARTICLE_DATA() = default;
    ARTICLE_DATA(shared_str id, ALife::_TIME_ID time, EArticleType articleType) : article_id{id}, receive_time{time}, article_type{articleType} {}

    void load(IReader& stream);
    void save(IWriter&) const;

    shared_str article_id{};
    ALife::_TIME_ID receive_time{};
    EArticleType article_type{eEncyclopediaArticle};
    bool readed{};
};

template <typename M>
struct object_loader::default_load<ARTICLE_DATA, M>
{
    void operator()(ARTICLE_DATA& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<ARTICLE_DATA, M>
{
    void operator()(const ARTICLE_DATA& data, M& stream) const { data.save(stream); }
};

DEFINE_VECTOR(shared_str, ARTICLE_ID_VECTOR, ARTICLE_ID_IT);
DEFINE_VECTOR(ARTICLE_DATA, ARTICLE_VECTOR, ARTICLE_IT);

class FindArticleByIDPred
{
public:
    FindArticleByIDPred(shared_str id) { object_id = id; }
    bool operator()(const ARTICLE_DATA& item)
    {
        if (item.article_id == object_id)
            return true;
        else
            return false;
    }

private:
    shared_str object_id;
};
