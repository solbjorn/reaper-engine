#pragma once

struct IPhysicShellCreator : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPhysicShellCreator);

public:
    virtual void CreatePhysicsShell() = 0;
};
