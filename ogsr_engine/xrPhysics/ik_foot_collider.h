#pragma once

#include "ik_collide_data.h"

class CGameObject;

class ik_pick_query
{
public:
    constexpr ik_pick_query()
        : _pos{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()},
          _dir{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()},
          _range{-std::numeric_limits<float>::max()}, _point{ik_foot_geom::none}
    {}

    constexpr explicit ik_pick_query(ik_foot_geom::e_collide_point point, const Fvector& pos, const Fvector& dir, float range)
        : _pos{pos}, _dir{dir}, _range{range}, _point{point}
    {
        XR_DEBUG_ASSERT(is_valid());
    }

    [[nodiscard]] constexpr bool is_valid() const
    {
#ifdef DEBUG
        if (point() != ik_foot_geom::none)
        {
            XR_ASSERT(range() >= 0.0f);
            XR_ASSERT(fsimilar(dir().magnitude(), 1.0f));

            return true;
        }

        return false;
#else
        return point() != ik_foot_geom::none;
#endif
    }

    [[nodiscard]] constexpr bool is_equal(const ik_pick_query& q) const
    {
        XR_DEBUG_ASSERT(q.is_valid());
        return is_valid() && q.point() == point() && fsimilar(q.range(), range()) && q.pos().similar(pos()) && q.dir().similar(dir());
    }

    [[nodiscard]] constexpr const Fvector& pos() const { return _pos; }
    [[nodiscard]] constexpr const Fvector& dir() const { return _dir; }
    [[nodiscard]] constexpr f32 range() const { return _range; }
    [[nodiscard]] constexpr ik_foot_geom::e_collide_point point() const { return _point; }

private:
    Fvector _pos;
    Fvector _dir;
    float _range;
    ik_foot_geom::e_collide_point _point;
};

class ik_foot_collider
{
    ik_pick_query previous_toe_query;
    ik_pick_query previous_heel_query;
    ik_pick_query previous_side_query;

    SIKCollideData previous_data;

public:
    ik_foot_collider();
    void collide(SIKCollideData& cld, const ik_foot_geom& foot_geom, CGameObject* O);
};

constexpr inline float collide_dist{0.5f};
