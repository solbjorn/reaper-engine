#ifndef ParticleCustom_included
#define ParticleCustom_included

class XR_NOVTABLE IParticleCustom : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IParticleCustom);

public:
    virtual ~IParticleCustom() = 0;

    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;

    virtual void UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM) = 0;
    virtual void OnFrame(u32 dt) = 0;

    virtual void Play() = 0;
    virtual void Stop(BOOL bDefferedStop = TRUE) = 0;
    virtual BOOL IsPlaying() = 0;

    virtual u32 ParticlesCount() = 0;

    virtual float GetTimeLimit() = 0;
    virtual BOOL IsLooped() { return GetTimeLimit() < 0.f; }

    virtual const shared_str Name() = 0;
    virtual void SetHudMode(BOOL b) = 0;
    virtual BOOL GetHudMode() = 0;
};

inline IParticleCustom::~IParticleCustom() = default;

#endif //	ParticleCustom_included
