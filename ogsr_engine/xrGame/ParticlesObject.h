#ifndef ParticlesObjectH
#define ParticlesObjectH

#include "..\xr_3da\PS_instance.h"

constexpr inline Fvector zero_vel{};

class CParticlesObject : public CPS_Instance
{
    RTTI_DECLARE_TYPEINFO(CParticlesObject, CPS_Instance);

private:
    using inherited = CPS_Instance;

    u32 dwLastTime;
    void Init(LPCSTR p_name, sector_id_t sector_id, BOOL bAutoRemove);
    void UpdateSpatial();

protected:
    bool m_bLooped; // флаг, что система зациклена
    bool m_bStopping; // вызвана функция Stop()

    u32 mt_dt;

public:
    explicit CParticlesObject(LPCSTR p_name, BOOL bAutoRemove, bool destroy_on_game_load);
    ~CParticlesObject() override;

    virtual bool shedule_Needed() { return true; }
    virtual float shedule_Scale() const;
    tmc::task<void> shedule_Update(u32 dt) override;
    void renderable_Render(u32 context_id, IRenderable* root) override;

private:
    void PerformAllTheWork();
    tmc::task<void> PerformAllTheWork_mt();

public:
    Fvector& Position() const;
    void SetXFORM(const Fmatrix& m);
    IC Fmatrix& XFORM() { return renderable.xform; }
    void UpdateParent(const Fmatrix& m, const Fvector& vel);

    void play_at_pos(const Fvector& pos, BOOL xform = FALSE);
    virtual void Play(BOOL hudMode = FALSE);
    void Stop(BOOL bDefferedStop = TRUE);
    virtual BOOL Locked() { return mt_dt; }

    bool IsLooped() { return m_bLooped; }
    bool IsAutoRemove();
    bool IsPlaying();
    void SetAutoRemove(bool auto_remove);
    int LifeTime() const { return m_iLifeTime; }

    const shared_str Name();

public:
    static CParticlesObject* Create(LPCSTR p_name, BOOL bAutoRemove = TRUE, bool remove_on_game_load = true)
    {
        return xr_new<CParticlesObject>(p_name, bAutoRemove, remove_on_game_load);
    }

    static void Destroy(CParticlesObject*& p)
    {
        if (p)
        {
            p->PSI_destroy();
            p = nullptr;
        }
    }
};

#endif // ParticlesObjectH
