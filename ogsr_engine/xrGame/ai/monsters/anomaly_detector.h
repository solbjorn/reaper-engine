#pragma once

#include "../xr_3da/Feel_Touch.h"
#include "../../CustomZone.h"

class CCustomMonster;

class CAnomalyDetector final : public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(CAnomalyDetector, Feel::Touch);

public:
    CCustomMonster* m_object;

    float m_radius;
    u32 m_time_to_rememeber;
    float m_detect_probability;

    bool m_active;
    bool m_forced;

private:
    xr_vector<CLASS_ID> m_ignore_clsids;

    struct SAnomalyInfo final
    {
        u16 id;
        bool ignored;
        u32 time_registered;
    };

    struct remove_predicate final
    {
        u32 time_remember;
        remove_predicate(u32 time) : time_remember(time) {}

        IC bool operator()(const SAnomalyInfo& info) { return (info.time_registered + time_remember < Device.dwTimeGlobal); }
    };

    using ANOMALY_INFO_VEC = xr_vector<SAnomalyInfo>;
    using ANOMALY_INFO_VEC_IT = ANOMALY_INFO_VEC::iterator;
    ANOMALY_INFO_VEC m_storage;

public:
    explicit CAnomalyDetector(CCustomMonster* monster);
    ~CAnomalyDetector() override;

    void load(LPCSTR section);
    void reinit();

    void update_schedule();
    void on_contact(CObject*);
    [[nodiscard]] BOOL feel_touch_contact(CObject*) override;

    void activate(bool = false);
    void deactivate(bool = false);
    void remove_all_restrictions();
    void remove_restriction(u16);
};
