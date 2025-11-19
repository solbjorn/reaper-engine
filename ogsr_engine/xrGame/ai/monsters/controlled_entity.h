#pragma once

#include "controller/controller.h"

class CEntity;

enum ETask : u32
{
    eTaskFollow = u32(0),
    eTaskAttack,
    eTaskNone = u32(-1)
};

struct SControlledInfo
{
    const CEntity* m_object;
    ETask m_task;
    Fvector m_position;
    u32 m_node;
    float m_radius;
};

class XR_NOVTABLE CControlledEntityBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CControlledEntityBase);

public:
    ~CControlledEntityBase() override = 0;

    virtual bool is_under_control() = 0;

    virtual void set_data(const SControlledInfo& info) = 0;
    virtual SControlledInfo& get_data() = 0;

    virtual void set_task_follow(const CEntity* e) = 0;
    virtual void set_task_attack(const CEntity* e) = 0;

    virtual void set_under_control(CController* controller) = 0;
    virtual void free_from_control() = 0;

    virtual void on_reinit() = 0;
    virtual void on_die() = 0;
    virtual void on_destroy() = 0;
};

inline CControlledEntityBase::~CControlledEntityBase() = default;

template <typename _Object>
class CControlledEntity : public CControlledEntityBase
{
    RTTI_DECLARE_TYPEINFO(CControlledEntity<_Object>, CControlledEntityBase);

public:
    SControlledInfo m_data;

    struct SGroupID
    {
        int team_id;
        int squad_id;
        int group_id;
    } saved_id;

    _Object* m_object;
    CController* m_controller;

    ~CControlledEntity() override = default;

    virtual bool is_under_control() { return !!m_controller; }

    virtual void set_data(const SControlledInfo& info) { m_data = info; }
    virtual SControlledInfo& get_data() { return m_data; }

    virtual void set_task_follow(const CEntity* e);
    virtual void set_task_attack(const CEntity* e);

    virtual void set_under_control(CController* controller);
    virtual void free_from_control();

    virtual void on_reinit();
    virtual void on_die();
    virtual void on_destroy();

    void init_external(_Object* obj) { m_object = obj; }

    CController* get_controller() { return m_controller; }
};

#include "controlled_entity_inline.h"
