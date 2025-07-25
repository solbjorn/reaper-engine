////////////////////////////////////////////////////////////////////////////
//	Module 		: steering_behaviour.h
//	Created 	: 25.04.2008
//  Modified 	: 25.04.2008
//	Author		: Lain
//	Description : steering behaviour classes
////////////////////////////////////////////////////////////////////////////

#ifndef STEERING_BEHAVIOUR_INCLUDED
#define STEERING_BEHAVIOUR_INCLUDED

//----------------------------------------------------------
// Steering Behaviour Namespace
//----------------------------------------------------------

namespace steering_behaviour
{

//----------------------------------------------------------
// Helper and Types
//----------------------------------------------------------

typedef Fvector vec;
typedef const vec& vec_arg;

namespace detail
{
vec random_vec();

} // namespace detail

//----------------------------------------------------------
// base
//----------------------------------------------------------

class base : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(base);

public:
    struct params : public virtual RTTI::Enable
    {
        RTTI_DECLARE_TYPEINFO(params);

    public:
        bool enabled;
        vec factor;
        float min_factor_dist;

        params(vec_arg factor, float min_factor_dist = base::s_min_factor_dist) : enabled(true), factor(factor), min_factor_dist(min_factor_dist) {}

        virtual bool update() = 0;
        virtual ~params() {}
    };

public:
    base(params* params_) : m_p_params(params_) {}

    virtual vec calc_acceleration() = 0;

    void set_enabled(bool value) { m_p_params->enabled = value; }
    bool is_enabled() const { return m_p_params->enabled; }
    params* get_supplier() { return m_p_params; }

    virtual ~base() { xr_delete(m_p_params); }

protected:
    float calc_dist_factor(float dist) const;
    float calc_dist_factor(vec_arg factor, float dist) const;

    const static int s_min_factor_dist = 1;

private:
    params* m_p_params;
};

//----------------------------------------------------------
// evade
//----------------------------------------------------------

class evade : public base
{
    RTTI_DECLARE_TYPEINFO(evade, base);

public:
    struct params : base::params
    {
        RTTI_DECLARE_TYPEINFO(params, base::params);

    public:
        vec pos;
        vec dest;
        float max_evade_range;
        vec (*pf_random_dir)(); // randomizer func in case we're in 0 dist from dest

        params(float max_evade_range, vec_arg factor, float min_factor_dist = base::s_min_factor_dist, vec (*pf_random_dir)() = &detail::random_vec)
            : base::params(factor, min_factor_dist), max_evade_range(max_evade_range), pf_random_dir(pf_random_dir)
        {}

        virtual bool update() = 0;
        virtual ~params() {}
    };

public:
    evade(params* params_) : base(params_), m_p_params(params_) {}

    virtual vec calc_acceleration();

private:
    params* m_p_params;
};

//----------------------------------------------------------
// pursue
//----------------------------------------------------------

class pursue : public base
{
    RTTI_DECLARE_TYPEINFO(pursue, base);

public:
    struct params : base::params
    {
        RTTI_DECLARE_TYPEINFO(params, base::params);

    public:
        vec pos;
        vec dest;
        float min_range2dest{};
        float change_vel_range;
        float arrive_vel;
        float arrive_range;
        float vel{};

        params(vec_arg factor, float arrive_range, float change_vel_range, float arrive_vel, float min_factor_dist = base::s_min_factor_dist)
            : base::params(factor, min_factor_dist), arrive_range(arrive_range), change_vel_range(change_vel_range), arrive_vel(arrive_vel)
        {}

        virtual bool update() = 0;
        virtual ~params() {}
    };

public:
    pursue(params* params_) : base(params_), m_p_params(params_) {}

    virtual vec calc_acceleration();

private:
    params* m_p_params;
};

//----------------------------------------------------------
// restrictor
//----------------------------------------------------------

class restrictor : public base
{
    RTTI_DECLARE_TYPEINFO(restrictor, base);

public:
    struct params : base::params
    {
        RTTI_DECLARE_TYPEINFO(params, base::params);

    public:
        vec pos;
        vec restrictor_pos;
        float max_allowed_range;

        params(vec_arg restrictor_pos, float max_allowed_range, vec_arg factor) : base::params(factor), restrictor_pos(restrictor_pos), max_allowed_range(max_allowed_range) {}

        virtual bool update() = 0;
        virtual ~params() {}
    };

public:
    restrictor(params* params_) : base(params_), m_p_params(params_) {}

    virtual vec calc_acceleration();

private:
    params* m_p_params;
};

//----------------------------------------------------------
// wander
//----------------------------------------------------------

class wander : public base
{
    RTTI_DECLARE_TYPEINFO(wander, base);

public:
    struct params : base::params
    {
        RTTI_DECLARE_TYPEINFO(params, base::params);

    public:
        enum plane_t
        {
            xy_plane,
            xz_plane,
            yz_plane
        } plane;

        float conservativeness; // how much we're taking current dir into account?
        float angle_change; // how big angle changes can be? (radians)
        vec dir;

        params(plane_t plane, float conservativeness, float angle_change, float factor)
            : base::params(cr_fvector3(factor, 0, 0)), plane(plane), conservativeness(conservativeness), angle_change(angle_change)
        {}

        virtual bool update() = 0;
        virtual ~params() {}
    };

public:
    wander(params* params_) : base(params_), m_p_params(params_), m_wander_angle(0) {}

    virtual vec calc_acceleration();

private:
    float& proj_x(vec& v);
    float& proj_y(vec& v);
    const float& proj_x(const vec& v);
    const float& proj_y(const vec& v);

    params* m_p_params;
    float m_wander_angle;
};

//----------------------------------------------------------
// containment
//----------------------------------------------------------

class containment : public base
{
    RTTI_DECLARE_TYPEINFO(containment, base);

public:
    struct params : base::params
    {
        RTTI_DECLARE_TYPEINFO(params, base::params);

    public:
        vec pos;
        vec dir;
        vec up;
        float turn_factor;
        vec thrust_factor;

        typedef xr_vector<vec> Probes;
        Probes probes; // obstacle-scanners (vectors in local (dir,up) space)

        params(float turn_factor, vec_arg thrust_factor, float min_factor_dist = base::s_min_factor_dist)
            : base::params(thrust_factor, min_factor_dist), turn_factor(turn_factor), thrust_factor(thrust_factor)
        {}

        virtual bool update() = 0;
        virtual bool test_obstacle(const vec& dest, vec& obstacle, vec& normal) = 0;

        virtual ~params() {}
    };

public:
    containment(params* params_) : base(params_), m_p_params(params_) {}

    virtual vec calc_acceleration();

private:
    params* m_p_params;
};

//----------------------------------------------------------
// grouping: cohesion + separation
//----------------------------------------------------------

class grouping : public base
{
    RTTI_DECLARE_TYPEINFO(grouping, base);

public:
    struct params : base::params
    {
        RTTI_DECLARE_TYPEINFO(params, base::params);

    public:
        vec pos;
        vec cohesion_factor;
        vec separation_factor;
        float max_separate_range;
        vec (*pf_random_dir)();

        params(vec_arg cohesion_factor, vec_arg separation_factor, float max_separate_range, float min_factor_dist = base::s_min_factor_dist,
               vec (*pf_random_dir)() = &detail::random_vec)
            : base::params(separation_factor, min_factor_dist), pf_random_dir(pf_random_dir), cohesion_factor(cohesion_factor), separation_factor(separation_factor),
              max_separate_range(max_separate_range)
        {}

        // this function should supply nearest object to group with
        virtual void first_nearest(vec& v) = 0; // start supplying nearest
        virtual bool nomore_nearest() = 0;
        virtual void next_nearest(vec& v) = 0; // next nearest, false if finished

        virtual bool update() = 0;
        virtual ~params() {}
    };

public:
    grouping(params* params_) : base(params_), m_p_params(params_) {}

    virtual vec calc_acceleration();

private:
    params* m_p_params;
};

//----------------------------------------------------------
// manager
//----------------------------------------------------------

class manager : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(manager);

public:
    vec calc_acceleration();
    void add(base* behaviour);
    void schedule_remove(base* behaviour);
    void clear();

    virtual ~manager() { clear(); }

protected:
    void remove_scheduled();
    static void deleter(base* p);

    typedef xr_set<base*> Behaviours;

    Behaviours m_behaviours;
    Behaviours m_schedule_remove;
};

} // namespace steering_behaviour

#endif
