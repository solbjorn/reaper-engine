#pragma once

class ICollisionForm;

class XR_NOVTABLE ICollidable : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ICollidable);

public:
    struct
    {
        ICollisionForm* model{};
    } collidable;

public:
    ICollidable() = default;
    virtual ~ICollidable();
};
