#ifndef PH_SKELETON_H
#define PH_SKELETON_H

#include "PHDefs.h"
#include "PHDestroyableNotificate.h"

class CPhysicsShellHolder;
class CSE_ALifePHSkeletonObject;
class CSE_Abstract;
class CSE_PHSkeleton;
class NET_Packet;

class XR_NOVTABLE CPHSkeleton : public CPHDestroyableNotificate
{
    RTTI_DECLARE_TYPEINFO(CPHSkeleton, CPHDestroyableNotificate);

public:
    PHSHELL_PAIR_VECTOR m_unsplited_shels;
    shared_str m_startup_anim;

    static u32 existence_time;
    u32 m_remove_time;

    Flags8 m_flags;
    bool b_removing;

private:
    // Creating
    void Init();

    void ClearUnsplited();
    // Splitting

    void PHSplit();

    void SpawnCopy();
    // Autoremove
    bool ReadyForRemove();
    void RecursiveBonesCheck(u16 id);

    void UnsplitSingle(CPHSkeleton* SO);

protected:
    virtual CPhysicsShellHolder* PPhysicsShellHolder() = 0;
    [[nodiscard]] virtual CPHSkeleton* PHSkeleton() { return this; }
    virtual void SpawnInitPhysics(CSE_Abstract* D) = 0;
    virtual void SaveNetState(NET_Packet&);
    virtual void RestoreNetState(CSE_PHSkeleton*);
    virtual void InitServerObject(CSE_Abstract* D); //
    virtual void CopySpawnInit();
    void RespawnInit(); // net_Destroy
    [[nodiscard]] bool Spawn(CSE_Abstract* D); // net_spawn
    void Update(u32); // shedule update
    void Load(LPCSTR section); // client load
    void SyncNetState();

public:
    CPHSkeleton();
    virtual ~CPHSkeleton();

    void SetAutoRemove(u32 time = existence_time);
    void SetNotNeedSave();
    [[nodiscard]] bool IsRemoving() const { return b_removing; }
    [[nodiscard]] u32 DefaultExitenceTime() const { return existence_time; }
};

#endif
