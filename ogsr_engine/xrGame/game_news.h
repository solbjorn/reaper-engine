#pragma once

#include "alife_space.h"
#include "object_loader.h"
#include "object_saver.h"
#include "string_table_defs.h"

constexpr int DEFAULT_NEWS_SHOW_TIME{5000};

struct GAME_NEWS_DATA
{
public:
    enum eNewsType
    {
        eNews = 0,
        eTalk = 1,
    } m_type{eNews};

    int show_time{DEFAULT_NEWS_SHOW_TIME};
    shared_str news_text;

    ALife::_TIME_ID receive_time{};

    shared_str texture_name;
    Frect tex_rect{};

    void save(IWriter&) const;
    void load(IReader&);

    LPCSTR SingleLineText();

private:
    // полный текст новостей, формируется при первом обращении
    xr_string full_news_text;
};

template <typename M>
struct object_loader::default_load<GAME_NEWS_DATA, M>
{
    void operator()(GAME_NEWS_DATA& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<GAME_NEWS_DATA, M>
{
    void operator()(const GAME_NEWS_DATA& data, M& stream) const { data.save(stream); }
};

DEFINE_DEQUE(GAME_NEWS_DATA, GAME_NEWS_VECTOR, GAME_NEWS_IT);
