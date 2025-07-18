#pragma once

#include "..\Include/xrRender/KinematicsAnimated.h"

class CPhysicsShell;

class interactive_motion : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(interactive_motion);

public:
    MotionID motion;
    CPhysicsShell* shell{};

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
    virtual void move_update(CPhysicsShell* s);
    virtual void collide(CPhysicsShell* s);
    virtual void state_end(CPhysicsShell* s);
    virtual void state_start(CPhysicsShell* s);
};

class imotion_position : public interactive_motion
{
    RTTI_DECLARE_TYPEINFO(imotion_position, interactive_motion);

public:
    typedef interactive_motion inherited;
    virtual void move_update(CPhysicsShell* s);
    virtual void collide(CPhysicsShell* s);
    virtual void state_end(CPhysicsShell* s);
    virtual void state_start(CPhysicsShell* s);
};
