#pragma once

class smapvis : public R_feedback
{
    RTTI_DECLARE_TYPEINFO(smapvis, R_feedback);

public:
    enum
    {
        state_counting = 0,
        state_working = 1,
        state_usingTC = 3,
    } state;

    xr_vector<dxRender_Visual*> invisible;

    dxRender_Visual* testQ_V;
    u32 frame_sleep;
    u32 test_count;
    u32 test_current;
    u32 testQ_id;
    u32 testQ_frame;
    ctx_id_t id{R__INVALID_CTX_ID};
    bool pending;

public:
    smapvis();
    ~smapvis() override;

    void invalidate();
    void begin(); // should be called before 'marker++' and before graph-build
    void end();
    void mark();
    void flushoccq(); // should be called when no rendering of light is supposed

    void resetoccq();

    IC bool sleep() { return Device.dwFrame > frame_sleep; }
    void rfeedback_static(dxRender_Visual* V) override;
};
