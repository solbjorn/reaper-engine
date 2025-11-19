////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_evaluators.h
//	Created 	: 24.04.2004
//  Modified 	: 24.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover evaluators
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restricted_object.h"
#include "game_graph_space.h"

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBase
//////////////////////////////////////////////////////////////////////////

class CCoverPoint;

class CCoverEvaluatorBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorBase);

protected:
    const CCoverPoint* m_selected{};
    u32 m_last_update{};
    u32 m_inertia_time{};
    float m_best_value{std::numeric_limits<float>::max()};
    bool m_initialized{};
    bool m_actuality{true};
    Fvector m_start_position{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    float m_last_radius{std::numeric_limits<float>::max()};
    CRestrictedObject* m_object;
    std::function<bool(const CCoverPoint*)> m_callback;

public:
    inline explicit CCoverEvaluatorBase(CRestrictedObject* object);
    ~CCoverEvaluatorBase() override = default;

    IC const CCoverPoint* selected() const;
    IC bool inertia(float radius);
    IC bool initialized() const;
    IC void setup(const std::function<bool(const CCoverPoint*)>& = {});
    IC void set_inertia(u32 inertia_time);
    IC void initialize(const Fvector& start_position, bool fake_call = false);
    virtual void finalize();
    IC bool accessible(const Fvector& position);
    IC bool actual() const;
    IC CRestrictedObject& object() const;
    IC void invalidate();
    IC float best_value() const;
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorCloseToEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorCloseToEnemy : public CCoverEvaluatorBase
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorCloseToEnemy, CCoverEvaluatorBase);

protected:
    typedef CCoverEvaluatorBase inherited;

protected:
    Fvector m_enemy_position;
    float m_min_distance;
    float m_max_distance;
    float m_current_distance;
    float m_deviation;
    float m_best_distance;

public:
    inline explicit CCoverEvaluatorCloseToEnemy(CRestrictedObject* object);
    ~CCoverEvaluatorCloseToEnemy() override = default;

    IC void initialize(const Fvector& start_position, bool fake_call = false);
    IC void setup(const Fvector& enemy_position, float min_enemy_distance, float max_enemy_distance, float deviation = 0.f, const std::function<bool(const CCoverPoint*)>& = {});
    void evaluate(const CCoverPoint* cover_point, float);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorFarFromEnemy : public CCoverEvaluatorCloseToEnemy
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorFarFromEnemy, CCoverEvaluatorCloseToEnemy);

protected:
    typedef CCoverEvaluatorCloseToEnemy inherited;

public:
    inline explicit CCoverEvaluatorFarFromEnemy(CRestrictedObject* object);
    ~CCoverEvaluatorFarFromEnemy() override = default;

    void evaluate(const CCoverPoint* cover_point, float);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBest
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBest : public CCoverEvaluatorCloseToEnemy
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorBest, CCoverEvaluatorCloseToEnemy);

protected:
    typedef CCoverEvaluatorCloseToEnemy inherited;

public:
    inline explicit CCoverEvaluatorBest(CRestrictedObject* object);
    ~CCoverEvaluatorBest() override = default;

    void evaluate(const CCoverPoint* cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBestByTime
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBestByTime : public CCoverEvaluatorBest
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorBestByTime, CCoverEvaluatorBest);

protected:
    typedef CCoverEvaluatorBest inherited;

public:
    inline explicit CCoverEvaluatorBestByTime(CRestrictedObject* object);
    ~CCoverEvaluatorBestByTime() override = default;

    void evaluate(const CCoverPoint* cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAngle
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorAngle : public CCoverEvaluatorCloseToEnemy
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorAngle, CCoverEvaluatorCloseToEnemy);

protected:
    typedef CCoverEvaluatorCloseToEnemy inherited;

protected:
    Fvector m_direction;
    Fvector m_best_direction;
    float m_best_alpha;
    u32 m_level_vertex_id;

public:
    inline explicit CCoverEvaluatorAngle(CRestrictedObject* object);
    ~CCoverEvaluatorAngle() override = default;

    IC void setup(const Fvector& enemy_position, float min_enemy_distance, float max_enemy_distance, u32 level_vertex_id, const std::function<bool(const CCoverPoint*)>& = {});
    void initialize(const Fvector& start_position, bool fake_call = false);
    void evaluate(const CCoverPoint* cover_point, float);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorSafe
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorSafe : public CCoverEvaluatorBase
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorSafe, CCoverEvaluatorBase);

protected:
    typedef CCoverEvaluatorBase inherited;

protected:
    float m_min_distance;

public:
    inline explicit CCoverEvaluatorSafe(CRestrictedObject* object);
    ~CCoverEvaluatorSafe() override = default;

    IC void setup(float min_distance, const std::function<bool(const CCoverPoint*)>& = {});
    void evaluate(const CCoverPoint* cover_point, float);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorRandomGame
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorRandomGame : public CCoverEvaluatorBase
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorRandomGame, CCoverEvaluatorBase);

protected:
    typedef CCoverEvaluatorBase inherited;

protected:
    GameGraph::_GRAPH_ID m_game_vertex_id;
    Fvector m_start_position;
    float m_max_distance_sqr;
    xr_vector<const CCoverPoint*> m_covers;

public:
    inline explicit CCoverEvaluatorRandomGame(CRestrictedObject* object);
    ~CCoverEvaluatorRandomGame() override = default;

    void setup(GameGraph::_GRAPH_ID game_vertex_id, float max_distance, const std::function<bool(const CCoverPoint*)>& = {});
    void evaluate(const CCoverPoint* cover_point, float);
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAmbush
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorAmbush : public CCoverEvaluatorBase
{
    RTTI_DECLARE_TYPEINFO(CCoverEvaluatorAmbush, CCoverEvaluatorBase);

protected:
    typedef CCoverEvaluatorBase inherited;

private:
    Fvector m_my_position;
    Fvector m_enemy_position;
    float m_min_enemy_distance;

public:
    inline explicit CCoverEvaluatorAmbush(CRestrictedObject* object);
    ~CCoverEvaluatorAmbush() override = default;

    void setup(const Fvector& my_position, const Fvector& enemy_position, float min_enemy_distance, const std::function<bool(const CCoverPoint*)>& = {});
    void evaluate(const CCoverPoint* cover_point, float);
};

#include "cover_evaluators_inline.h"
