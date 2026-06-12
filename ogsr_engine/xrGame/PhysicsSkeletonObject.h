#ifndef PHYSICS_SKELETON_OBJECT_H
#define PHYSICS_SKELETON_OBJECT_H

#include "physicsshellholder.h"
#include "PHSkeleton.h"

class CSE_ALifePHSkeletonObject;

class CPhysicsSkeletonObject : public CPhysicsShellHolder, public CPHSkeleton
{
    RTTI_DECLARE_TYPEINFO(CPhysicsSkeletonObject, CPhysicsShellHolder, CPHSkeleton);

public:
    typedef CPhysicsShellHolder inherited;

    CPhysicsSkeletonObject();
    ~CPhysicsSkeletonObject() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void Load(gsl::czstring section) override;
    tmc::task<void> UpdateCL() override; // Called each frame, so no need for dt
    tmc::task<void> shedule_Update(u32 dt) override; //
    void net_Save(NET_Packet& P) override;
    [[nodiscard]] BOOL net_SaveRelevant() override;
    [[nodiscard]] BOOL UsedAI_Locations() override;

protected:
    [[nodiscard]] CPhysicsShellHolder* PPhysicsShellHolder() override { return PhysicsShellHolder(); }
    [[nodiscard]] CPHSkeleton* PHSkeleton() override { return this; }
    void SpawnInitPhysics(CSE_Abstract* D) override;
    virtual void PHObjectPositionUpdate();
    virtual void CreatePhysicsShell(CSE_Abstract* e);
};

#endif
