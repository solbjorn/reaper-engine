//---------------------------------------------------------------------------
#ifndef particle_managerH
#define particle_managerH
//---------------------------------------------------------------------------

#include "particle_actions.h"

namespace PAPI
{
class CParticleManager : public IParticleManager
{
    RTTI_DECLARE_TYPEINFO(CParticleManager, IParticleManager);

private:
    // These are static because all threads access the same effects.
    // All accesses to these should be locked.
    using ParticleEffectVec = xr_vector<ParticleEffect*>;
    using ParticleActionsVec = xr_vector<ParticleActions*>;

    ParticleEffectVec m_effect_vec;
    ParticleActionsVec m_alist_vec;

    std::mutex pm_Locked;

public:
    CParticleManager();
    ~CParticleManager() override;

    // Return an index into the list of particle effects where
    ParticleEffect* GetEffectPtr(int effect_id);
    ParticleActions* GetActionListPtr(int alist_id);

    // create&destroy
    [[nodiscard]] s32 CreateEffect(u32 max_particles) override;
    void DestroyEffect(s32 effect_id) override;
    [[nodiscard]] s32 CreateActionList() override;
    void DestroyActionList(s32 alist_id) override;

    // control
    void PlayEffect(s32 alist_id) override;
    void StopEffect(s32 effect_id, s32 alist_id, BOOL deffered = TRUE) override;

    // update&render
    void Update(s32 effect_id, s32 alist_id, f32 dt) override;
    void Render(s32 effect_id) override;
    void Transform(s32 alist_id, const Fmatrix& m, const Fvector3& velocity) override;

    // effect
    void RemoveParticle(s32 effect_id, u32 p_id) override;
    void SetMaxParticles(s32 effect_id, u32 max_particles) override;
    void SetCallback(s32 effect_id, OnBirthParticleCB b, OnDeadParticleCB d, void* owner, u32 param) override;
    void GetParticles(s32 effect_id, Particle*& particles, u32& cnt) override;
    [[nodiscard]] u32 GetParticlesCount(s32 effect_id) override;

    // action
    [[nodiscard]] ParticleAction* CreateAction(PActionEnum action_id) override;
    [[nodiscard]] u32 LoadActions(s32 alist_id, IReader& R, bool copFormat) override;
};
} // namespace PAPI

//---------------------------------------------------------------------------

#endif
