#pragma once

class ICollisionForm;

class ICollidable
{
public:
    struct
    {
        ICollisionForm* model;
    } collidable;

public:
    ICollidable();
    virtual ~ICollidable();
};
