#pragma once

class CActor;

class CActorInputHandler : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CActorInputHandler);

public:
    CActorInputHandler() = default;
    ~CActorInputHandler() override = default;

    virtual void reinit();

    virtual void install();
    virtual void install(CActor*);
    virtual void release();

    virtual bool authorized(int) { return true; }
    virtual float mouse_scale_factor() { return 1.f; }

protected:
    CActor* m_actor;
};
