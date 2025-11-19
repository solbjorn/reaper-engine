#pragma once

enum ETaskState : u32
{
    eTaskStateFail = 0,
    eTaskStateInProgress,
    eTaskStateCompleted,
    //.	eTaskUserDefined,
    eTaskStateDummy = u32(-1)
};

typedef shared_str TASK_ID;
DEFINE_VECTOR(TASK_ID, TASK_ID_VECTOR, TASK_ID_IT);
extern shared_str g_active_task_id;
extern u16 g_active_task_objective_id;

#include "alife_abstract_registry.h"

class CGameTask;

struct SGameTaskKey
{
    CGameTask* game_task{};
    TASK_ID task_id{};

    SGameTaskKey() = default;
    SGameTaskKey(TASK_ID t_id) : task_id{t_id} {}

    void save(IWriter& stream) const;
    void load(IReader& stream);
    void destroy();

    void sync_task_version();
};

template <typename M>
struct object_loader::default_load<SGameTaskKey, M>
{
    void operator()(SGameTaskKey& data, M& stream) const { data.load(stream); }
};

template <typename M>
struct object_saver::default_save<SGameTaskKey, M>
{
    void operator()(const SGameTaskKey& data, M& stream) const { data.save(stream); }
};

template <>
struct object_destroyer::default_destroy<SGameTaskKey>
{
    void operator()(SGameTaskKey& data) const { data.destroy(); }
};

DEFINE_VECTOR(SGameTaskKey, GameTasks, GameTasks_it);

struct CGameTaskRegistry : public CALifeAbstractRegistry<u16, GameTasks>
{
    RTTI_DECLARE_TYPEINFO(CGameTaskRegistry, CALifeAbstractRegistry<u16, GameTasks>);

public:
    ~CGameTaskRegistry() override = default;

    virtual void save(IWriter& stream)
    {
        CALifeAbstractRegistry<u16, GameTasks>::save(stream);
        save_data(g_active_task_id, stream);
        save_data(g_active_task_objective_id, stream);
    }

    virtual void load(IReader& stream)
    {
        CALifeAbstractRegistry<u16, GameTasks>::load(stream);
        load_data(g_active_task_id, stream);
        load_data(g_active_task_objective_id, stream);
    }
};
