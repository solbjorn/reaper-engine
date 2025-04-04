//---------------------------------------------------------------------------
#ifndef ParticleEffectH
#define ParticleEffectH
//---------------------------------------------------------------------------

#include "ParticleEffectDef.h"

#include "../xrRender/FBasicVisual.h"
#include "../xrRender/dxParticleCustom.h"

namespace PS
{
class CParticleEffect : public dxParticleCustom
{
    //		friend void ParticleRenderStream( LPVOID lpvParams );
    friend class CPEDef;

protected:
    float m_fElapsedLimit;

    int m_HandleEffect;
    int m_HandleActionList;

    u32 m_MemDT;

    Fvector m_InitialPosition;

public:
    CPEDef* m_Def;
    Fmatrix m_XFORM;

protected:
    DestroyCallback m_DestroyCallback;
    CollisionCallback m_CollisionCallback;

public:
    enum
    {
        flRT_Playing = (1 << 0),
        flRT_DefferedStop = (1 << 1),
        flRT_XFORM = (1 << 2),
        flRT_HUDmode = (1 << 3),
    };
    Flags8 m_RT_Flags;

protected:
    void RefreshShader();

public:
    CParticleEffect();
    virtual ~CParticleEffect();

    void OnFrame(u32 dt);

    u32 RenderTO();
    virtual void Render(CBackend& cmd_list, float, bool) override;

private:
    void ParticleRenderStream(PAPI::Particle* particles, FVF::LIT* pv, u32 count);

public:
    virtual void Copy(dxRender_Visual* pFrom);

    virtual void OnDeviceCreate();
    virtual void OnDeviceDestroy();

    virtual void UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM);

    BOOL Compile(CPEDef* def);

    IC CPEDef* GetDefinition() const { return m_Def; }
    IC int GetHandleEffect() const { return m_HandleEffect; }
    IC int GetHandleActionList() const { return m_HandleActionList; }

    virtual void Play();
    virtual void Stop(BOOL bDefferedStop = TRUE);
    virtual BOOL IsPlaying() { return m_RT_Flags.is(flRT_Playing); }

    virtual void SetHudMode(BOOL b) { m_RT_Flags.set(flRT_HUDmode, b); }
    virtual BOOL GetHudMode() { return m_RT_Flags.is(flRT_HUDmode); }

    virtual float GetTimeLimit()
    {
        VERIFY(m_Def);
        return m_Def->m_Flags.is(CPEDef::dfTimeLimit) ? m_Def->m_fTimeLimit : -1.f;
    }

    virtual const shared_str Name()
    {
        VERIFY(m_Def);
        return m_Def->m_Name;
    }

    void SetDestroyCB(DestroyCallback destroy_cb) { m_DestroyCallback = destroy_cb; }
    void SetCollisionCB(CollisionCallback collision_cb) { m_CollisionCallback = collision_cb; }
    void SetBirthDeadCB(PAPI::OnBirthParticleCB bc, PAPI::OnDeadParticleCB dc, void* owner, u32 p) const;

    virtual u32 ParticlesCount();
};
void OnEffectParticleBirth(void* owner, u32 param, PAPI::Particle& m, u32 idx);
void OnEffectParticleDead(void* owner, u32 param, PAPI::Particle& m, u32 idx);

} // namespace PS
//---------------------------------------------------------------------------
#endif
