#ifndef I_COLLISION_DAMAGE_INFO_H
#define I_COLLISION_DAMAGE_INFO_H

class ICollisionHitCallback;

class XR_NOVTABLE ICollisionDamageInfo : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ICollisionDamageInfo);

public:
    ~ICollisionDamageInfo() override = 0;

    [[nodiscard]] virtual f32 ContactVelocity() const = 0;
    virtual void HitDir(Fvector& dir) const = 0;
    [[nodiscard]] virtual const Fvector& HitPos() const = 0;
    [[nodiscard]] virtual u16 DamageInitiatorID() const = 0;
    [[nodiscard]] virtual CObject* DamageInitiator() const = 0;
    [[nodiscard]] virtual ALife::EHitType HitType() const = 0;
    [[nodiscard]] virtual ICollisionHitCallback* HitCallback() const = 0;
    virtual void Reinit() = 0;
    virtual void SetInitiated() = 0;
    [[nodiscard]] virtual bool IsInitiated() const = 0;
    [[nodiscard]] virtual bool GetAndResetInitiated() = 0;
};

inline ICollisionDamageInfo::~ICollisionDamageInfo() = default;

#endif
