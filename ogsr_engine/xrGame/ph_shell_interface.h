#pragma once

struct XR_NOVTABLE IPhysicShellCreator : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPhysicShellCreator);

public:
    ~IPhysicShellCreator() override = 0;

    virtual void CreatePhysicsShell() = 0;
};

inline IPhysicShellCreator::~IPhysicShellCreator() = default;
