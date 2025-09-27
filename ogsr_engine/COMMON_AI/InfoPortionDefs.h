#pragma once

#include "alife_space.h"
#include "object_loader.h"
#include "object_saver.h"

struct INFO_DATA
{
    INFO_DATA() = default;
    INFO_DATA(shared_str id, ALife::_TIME_ID time) : info_id{id}, receive_time{time} {}

    void load(IReader& stream);
    void save(IWriter& stream) const;

    shared_str info_id{};
    // время получения нужно порции информации
    ALife::_TIME_ID receive_time{};
};

template <typename M>
struct object_loader::default_load<INFO_DATA, M>
{
    void operator()(INFO_DATA& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<INFO_DATA, M>
{
    void operator()(const INFO_DATA& data, M& stream) const { data.save(stream); }
};

DEFINE_VECTOR(INFO_DATA, KNOWN_INFO_VECTOR, KNOWN_INFO_VECTOR_IT);
