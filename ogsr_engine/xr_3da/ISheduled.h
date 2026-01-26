#pragma once

class XR_NOVTABLE ISheduled : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ISheduled);

public:
    struct
    {
        u32 t_min : 14; // minimal bound of update time (sample: 20ms)
        u32 t_max : 14; // maximal bound of update time (sample: 200ms)
        u32 b_RT : 1;
        u32 b_locked : 1;
    } shedule;

#ifdef DEBUG
    u32 dbg_startframe;
    u32 dbg_update_shedule;
#endif

    ISheduled();
    ~ISheduled() override;

    void shedule_register();
    void shedule_unregister(bool force = false);

    virtual float shedule_Scale() const = 0;

#ifdef DEBUG
    virtual tmc::task<void> shedule_Update(u32);
#else
    virtual tmc::task<void> shedule_Update(u32) { co_return; }
#endif

    virtual shared_str shedule_Name() const { return shared_str("unknown"); }
    virtual bool shedule_Needed() = 0;
};
