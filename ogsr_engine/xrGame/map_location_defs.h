#pragma once

#include "alife_abstract_registry.h"

class CMapLocation;

struct SLocationKey
{
    shared_str spot_type;
    CMapLocation* location{};
    u16 object_id{};
    bool actual{true};

    SLocationKey() = default;
    SLocationKey(shared_str s, u16 id) : spot_type{s}, object_id{id} {}

    bool operator<(const SLocationKey& key) const { return actual > key.actual; } // move non-actual to tail

    void save(IWriter& stream) const;
    void load(IReader& stream);
    void destroy();
};

template <typename M>
struct object_loader::default_load<SLocationKey, M>
{
    void operator()(SLocationKey& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<SLocationKey, M>
{
    void operator()(const SLocationKey& data, M& stream) const { data.save(stream); }
};

template <>
struct object_destroyer::default_destroy<SLocationKey>
{
    void operator()(SLocationKey& data) const { data.destroy(); }
};

DEFINE_VECTOR(SLocationKey, Locations, Locations_it);

struct CMapLocationRegistry : public CALifeAbstractRegistry<u16, Locations>
{
    virtual void save(IWriter& stream);
};
