#pragma once

#include "alife_abstract_registry.h"

extern xr_token actor_stats_token[];

struct SStatDetailBData
{
    shared_str key;
    s32 int_count;
    s32 int_points;
    shared_str str_value;

    void save(IWriter& stream) const;
    void load(IReader& stream);
};

template <typename M>
struct object_loader::default_load<SStatDetailBData, M>
{
    void operator()(SStatDetailBData& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<SStatDetailBData, M>
{
    void operator()(const SStatDetailBData& data, M& stream) const { data.save(stream); }
};

typedef xr_vector<SStatDetailBData> vStatDetailData;

struct SStatSectionData
{
    shared_str key;
    vStatDetailData data;

    SStatDetailBData& GetData(const shared_str&);
    void RemoveData(const shared_str&);
    s32 GetTotalPoints() const;
    s32 GetTotalCounts() const;

    void save(IWriter& stream) const;
    void load(IReader& stream);
};

template <typename M>
struct object_loader::default_load<SStatSectionData, M>
{
    void operator()(SStatSectionData& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<SStatSectionData, M>
{
    void operator()(const SStatSectionData& data, M& stream) const { data.save(stream); }
};

typedef xr_vector<SStatSectionData> vStatSectionData;
typedef CALifeAbstractRegistry<u16, vStatSectionData> CActorStatisticRegistry;
