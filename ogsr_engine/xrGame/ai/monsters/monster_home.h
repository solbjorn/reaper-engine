#pragma once

class CBaseMonster;
class CPatrolPath;

class CMonsterHome
{
    CBaseMonster* m_object;
    const CPatrolPath* m_path;
    u32 m_level_vertex_id;
    float m_radius_min;
    float m_radius_max;
    float m_radius_middle;
    u32 min_move_dist;
    u32 max_move_dist;

    bool m_aggressive;

public:
    CMonsterHome(CBaseMonster* obj) : m_object{obj} {}

    void load(LPCSTR line);
    void setup(LPCSTR path_name, float min_radius, float max_radius, bool aggressive = false, float middle_radius = 0.0f);
    void setup(u32 lv_ID, float min_radius, float max_radius, bool aggressive = false, float middle_radius = 0.0f);
    void remove_home();
    void set_move_dists(u32 min_dist, u32 max_dist);

    [[nodiscard]] u32 get_place_in_min_home();
    [[nodiscard]] u32 get_place_in_mid_home();
    XR_SYSV [[nodiscard]] u32 get_place_in_max_home_to_direction(Fvector to_direction);
    [[nodiscard]] u32 get_place_in_max_home();
    [[nodiscard]] u32 get_place();
    [[nodiscard]] u32 get_place_in_cover();
    [[nodiscard]] bool at_home();
    [[nodiscard]] bool at_home(const Fvector& pos);
    [[nodiscard]] bool at_home(const Fvector& pos, float radius);
    [[nodiscard]] bool at_min_home(const Fvector& pos);
    [[nodiscard]] bool at_mid_home(const Fvector& pos);
    XR_SYSV [[nodiscard]] Fvector get_home_point() const;
    [[nodiscard]] float get_min_radius() const { return m_radius_min; }
    [[nodiscard]] float get_mid_radius() const { return m_radius_middle; }
    [[nodiscard]] float get_max_radius() const { return m_radius_max; }
    [[nodiscard]] bool has_home() const { return m_path != nullptr || m_level_vertex_id != std::numeric_limits<u32>::max(); }
    [[nodiscard]] bool is_aggressive() const { return m_aggressive; }
    void setup(CPatrolPath*, float, float, bool = false, float = 0.0f);
};
