#pragma once

class CBlender_fluid_advect : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_advect, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_advect_velocity : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_advect_velocity, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_simulate : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_simulate, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_obst : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_obst, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_emitter : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_emitter, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid emitters"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_obstdraw : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_obstdraw, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_raydata : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_raydata, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};

class CBlender_fluid_raycast : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_raycast, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: 3dfluid maths 2"; }

    virtual void Compile(CBlender_Compile& C);
};
