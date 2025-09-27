#pragma once

class ICollisionForm;

class ICollidable : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ICollidable);

public:
    struct
    {
        ICollisionForm* model{};
    } collidable;

public:
    ICollidable();
    virtual ~ICollidable();
};
