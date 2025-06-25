#ifndef SoundRender_TargetAH
#define SoundRender_TargetAH

#include "SoundRender_Target.h"
#include "SoundRender_CoreA.h"

class CSoundRender_TargetA : public CSoundRender_Target
{
    RTTI_DECLARE_TYPEINFO(CSoundRender_TargetA, CSoundRender_Target);

public:
    using inherited = CSoundRender_Target;

    // OpenAL
    ALuint pSource{};
    ALuint pBuffers[sdef_target_count_submit]{};

    ALuint dataFormat;
    ALsizei sampleRate;

    float cache_gain{};
    float cache_pitch{1.f};

    float cache_low_gain{};
    float cache_high_gain{};

    ALuint sound_local_filter{};

private:
    void submit_buffer(ALuint BufferID) const;
    void submit_all_buffers() const;

public:
    bool _initialize() override;
    void _destroy() override;
    void _restart() override;

    void start(CSoundRender_Emitter* E) override;
    void render() override;
    void rewind() override;
    void stop() override;
    void update() override;
    void fill_parameters(CSoundRender_Core* core) override;

    void alAuxInit(ALuint slot) override;
};

#endif
