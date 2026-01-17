#ifndef RainH
#define RainH

// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#include "../xrcdb/xr_collide_defs.h"

// refs
class IRender_DetailModel;

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/RainRender.h"
//
class CEffect_Rain
{
    friend class dxRainRender;

private:
    struct Item
    {
        Fvector P;
        Fvector Phit;
        Fvector D;
        float fSpeed;
        u32 dwTime_Life;
        u32 dwTime_Hit;
        u32 uv_set;
        void invalidate() { dwTime_Life = 0; }
    };
    struct Particle
    {
        Particle *next, *prev;
        Fmatrix mXForm;
        Fsphere bounds;
        float time;
    };
    enum States
    {
        stIdle = 0,
        stWorking
    };

private:
    // Visualization	(rain) and (drops)
    FactoryPtr<IRainRender> m_pRender;
    /*
    // Visualization	(rain)
    ref_shader						SH_Rain;
    ref_geom						hGeom_Rain;

    // Visualization	(drops)
    IRender_DetailModel*			DM_Drop;
    ref_geom						hGeom_Drops;
    */

    // Data and logic
    xr_vector<Item> items;
    States state;

    // Particles
    xr_vector<Particle> particle_pool;
    Particle* particle_active;
    Particle* particle_idle;

    // Sounds
    ref_sound snd_Ambient;
    float rain_volume;
    float rain_hemi = 0.0f;

    // Utilities
    void p_create();
    void p_destroy();

    void p_remove(Particle* P, Particle*& LST);
    void p_insert(Particle* P, Particle*& LST);
    int p_size(Particle* LST);
    Particle* p_allocate();
    void p_free(Particle* P);

    // Some methods
    void Born(Item& dest, const float radius, const float speed, const float vel, const Fvector2& offset, const Fvector3& axis);
    void Hit(Fvector& pos);
    BOOL RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt);
    void RenewItem(Item& dest, float height, BOOL bHit);

public:
    CEffect_Rain();
    ~CEffect_Rain();

    void Render();
    tmc::task<void> Calculate(tmc::manual_reset_event& event);
    void OnFrame();

    void InvalidateState()
    {
        if (state != stIdle)
            snd_Ambient.stop();
        rain_volume = 0.0f;
        state = stIdle;
    }

    float GetRainVolume() { return rain_volume; }
    float GetRainHemi() { return rain_hemi; }
};

#endif // RainH
