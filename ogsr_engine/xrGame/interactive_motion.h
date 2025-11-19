#pragma once

#include "..\Include/xrRender/KinematicsAnimated.h"

class CPhysicsShell;

class XR_NOVTABLE interactive_motion : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(interactive_motion);

public:
    CPhysicsShell* shell{};
    MotionID motion;

protected:
    Flags8 flags;
    enum Flag
    {
        fl_use_death_motion = 1 << 4,
        fl_switch_dm_toragdoll = 1 << 5,
        fl_started = 1 << 6,
    };

public:
    interactive_motion();
    ~interactive_motion() override = 0;

    void init();
    void destroy();
    void setup(LPCSTR m, CPhysicsShell* s);
    void setup(MotionID m, CPhysicsShell* s);
    void update(CPhysicsShell* s);
    IC bool is_enabled() { return !!flags.test(fl_use_death_motion); }

    void play(CPhysicsShell* s);

private:
    virtual void move_update(CPhysicsShell* s) = 0;
    virtual void collide(CPhysicsShell* s) = 0;

protected:
    void state_end();
    virtual void state_end(CPhysicsShell* s);
    virtual void state_start(CPhysicsShell* s);

private:
    void switch_to_free(CPhysicsShell* s);
    static void anim_callback(CBlend* B);
};

inline interactive_motion::~interactive_motion() = default;

IC void destroy_motion(interactive_motion*& im)
{
    if (!im)
        return;

    im->destroy();
    xr_delete(im);
}

class imotion_velocity : public interactive_motion
{
    RTTI_DECLARE_TYPEINFO(imotion_velocity, interactive_motion);

public:
    typedef interactive_motion inherited;

    ~imotion_velocity() override = default;

    void move_update(CPhysicsShell* s) override;
    void collide(CPhysicsShell*) override;
    void state_end(CPhysicsShell* s) override;
    void state_start(CPhysicsShell* s) override;
};

class imotion_position : public interactive_motion
{
    RTTI_DECLARE_TYPEINFO(imotion_position, interactive_motion);

public:
    typedef interactive_motion inherited;

    ~imotion_position() override = default;

    void move_update(CPhysicsShell* s) override;
    void collide(CPhysicsShell* s) override;
    void state_end(CPhysicsShell* s) override;
    void state_start(CPhysicsShell* s) override;
};
