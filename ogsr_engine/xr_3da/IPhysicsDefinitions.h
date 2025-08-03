#pragma once

class XR_NOVTABLE IPhysicsGeometry : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPhysicsGeometry);

public:
    virtual ~IPhysicsGeometry() = 0;

    virtual void get_Box(Fmatrix& form, Fvector& sz) = 0;
    virtual bool collide_fluids() = 0;
};

inline IPhysicsGeometry::~IPhysicsGeometry() = default;

class XR_NOVTABLE IPhysicsElement : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPhysicsElement);

public:
    virtual ~IPhysicsElement() = 0;

    virtual const Fmatrix& XFORM() const = 0;
    virtual void get_LinearVel(Fvector& velocity) = 0;
    virtual void get_AngularVel(Fvector& velocity) = 0;
    virtual const Fvector& mass_Center() = 0;
    virtual u16 numberOfGeoms() = 0;
    virtual IPhysicsGeometry* geometry(u16 i) const = 0;
};

inline IPhysicsElement::~IPhysicsElement() = default;

class XR_NOVTABLE IPhysicsShell : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IPhysicsShell);

public:
    virtual ~IPhysicsShell() = 0;

    virtual const Fmatrix& XFORM() const = 0;
    virtual IPhysicsElement& IElement(u16 index) = 0;
    virtual u16 get_ElementsNumber() = 0;
};

inline IPhysicsShell::~IPhysicsShell() = default;

class XR_NOVTABLE IObjectPhysicsCollision : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IObjectPhysicsCollision);

public:
    virtual ~IObjectPhysicsCollision() = 0;

    virtual IPhysicsShell* physics_shell() const = 0;
    virtual IPhysicsElement* physics_character() const = 0; // depricated
};

inline IObjectPhysicsCollision::~IObjectPhysicsCollision() = default;
