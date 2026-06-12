#pragma once

class CBlender_fluid_advect : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_advect, IBlender);

public:
    ~CBlender_fluid_advect() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid maths"; }

    void Compile(CBlender_Compile& C) override;
};

class CBlender_fluid_advect_velocity : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_advect_velocity, IBlender);

public:
    ~CBlender_fluid_advect_velocity() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid maths"; }

    void Compile(CBlender_Compile& C) override;
};

class CBlender_fluid_simulate : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_simulate, IBlender);

public:
    ~CBlender_fluid_simulate() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid maths"; }

    void Compile(CBlender_Compile& C) override;
};

class CBlender_fluid_obst : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_obst, IBlender);

public:
    ~CBlender_fluid_obst() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid maths 2"; }

    void Compile(CBlender_Compile& C) override;
};

class CBlender_fluid_emitter : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_emitter, IBlender);

public:
    ~CBlender_fluid_emitter() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid emitters"; }

    void Compile(CBlender_Compile& C) override;
};

class CBlender_fluid_obstdraw : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_obstdraw, IBlender);

public:
    ~CBlender_fluid_obstdraw() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid maths 2"; }

    void Compile(CBlender_Compile& C) override;
};

class CBlender_fluid_raydata : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_raydata, IBlender);

public:
    ~CBlender_fluid_raydata() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid maths 2"; }

    void Compile(CBlender_Compile& C) override;
};

class CBlender_fluid_raycast : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_fluid_raycast, IBlender);

public:
    ~CBlender_fluid_raycast() override = default;

    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: 3dfluid maths 2"; }

    void Compile(CBlender_Compile& C) override;
};
