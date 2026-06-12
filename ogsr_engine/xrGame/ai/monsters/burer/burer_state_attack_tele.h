#pragma once

#include "../state.h"
#include "../../../grenade.h"

template <typename Object>
class CStateBurerAttackTele : public CState<Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBurerAttackTele<Object>, CState<Object>);

private:
    typedef CState<Object> inherited;
    using inherited::object;

    xr_vector<CPhysicsShellHolder*> tele_objects;
    CPhysicsShellHolder* selected_object;
    xr_vector<CObject*> m_nearest;

    u32 time_started{};

    enum
    {
        ACTION_TELE_STARTED,
        ACTION_TELE_CONTINUE,
        ACTION_TELE_FIRE,
        ACTION_WAIT_FIRE_END,
        ACTION_COMPLETED,
    } m_action;

public:
    explicit CStateBurerAttackTele(Object* obj);
    ~CStateBurerAttackTele() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;

private:
    void FindObjects();

    void HandleGrenades();

    void ExecuteTeleContinue();
    void ExecuteTeleFire();

    bool IsActiveObjects();

    bool CheckTeleStart();
    void SelectObjects();

    // internal for FindObjects
    void FindFreeObjects(xr_vector<CObject*>& tpObjects, const Fvector& pos);
    void OnGrenadeDestroyed(CGrenade* const grenade);

    void FireAllToEnemy();
    void deactivate();

private:
    TTime m_last_grenade_scan;
    TTime m_anim_end_tick;
    TTime m_end_tick{};
    float m_initial_health{};
};

#include "burer_state_attack_tele_inline.h"
