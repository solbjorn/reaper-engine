#ifndef PH_DESTROYABLE
#define PH_DESTROYABLE

#include "hit.h"

class CPhysicsShellHolder;
class CSE_Abstract;
class CPHDestroyableNotificate;

class XR_NOVTABLE CPHDestroyableNotificator : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPHDestroyableNotificator);

public:
    ~CPHDestroyableNotificator() override = 0;

    virtual void NotificateDestroy(CPHDestroyableNotificate* dn) = 0;
};

inline CPHDestroyableNotificator::~CPHDestroyableNotificator() = default;

class XR_NOVTABLE CPHDestroyable : public CPHDestroyableNotificator
{
    RTTI_DECLARE_TYPEINFO(CPHDestroyable, CPHDestroyableNotificator);

public:
    xr_vector<shared_str> m_destroyed_obj_visual_names;
    xr_vector<CPHDestroyableNotificate*> m_notificate_objects;
    u16 m_depended_objects;
    Flags8 m_flags;
    SHit m_fatal_hit;

    enum
    {
        fl_destroyable = 1 << 0,
        fl_destroyed = 1 << 1,
        fl_released = 1 << 2,
    };

    [[nodiscard]] virtual CPhysicsShellHolder* PPhysicsShellHolder() = 0;

public:
    CPHDestroyable();
    ~CPHDestroyable() override = 0;

    void Init();
    void RespawnInit();
    void SetFatalHit(const SHit& hit);
    void Destroy(u16 ref_id = u16(-1), LPCSTR section = "ph_skeleton_object");
    SHit& FatalHit() { return m_fatal_hit; }
    void Load(LPCSTR section);
    void Load(CInifile* ini, LPCSTR section);
    void NotificateDestroy(CPHDestroyableNotificate* dn) override;
    void PhysicallyRemoveSelf();
    IC bool Destroyable() { return !!m_flags.test(fl_destroyable); }
    IC bool Destroyed() { return !!m_flags.test(fl_destroyed); }
    IC bool CanDestroy() { return m_flags.test(fl_destroyable) && !m_flags.test(fl_destroyed); }
    [[nodiscard]] virtual bool CanRemoveObject() { return true; }
    virtual void SheduleUpdate(u32);
    virtual void GenSpawnReplace(u16 source_id, LPCSTR section, shared_str visual_name);
    virtual void InitServerObject(CSE_Abstract* D);

private:
    void NotificatePart(CPHDestroyableNotificate* dn);
    void PhysicallyRemovePart(CPHDestroyableNotificate* dn);
};

inline CPHDestroyable::~CPHDestroyable() = default;

#endif
