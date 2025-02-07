#pragma once

#include <al.h>
#include "soundrender.h"

class CSoundRender_Target
{
protected:
    CSoundRender_Emitter* m_pEmitter;
    BOOL rendering;

public:
    float priority{};
    bool bAlSoft = false;
    bool bEFX = false;

protected:
    OggVorbis_File ovf{};
    IReader* wave;
    void attach();
    void detach();

public:
    OggVorbis_File* get_data()
    {
        if (!wave)
            attach();
        return &ovf;
    }

    CSoundRender_Target();
    virtual ~CSoundRender_Target();

    CSoundRender_Emitter* get_emitter() const { return m_pEmitter; }
    BOOL get_Rendering() const { return rendering; }

    virtual BOOL _initialize() = 0;
    virtual void _destroy() = 0;
    virtual void _restart() = 0;

    virtual void start(CSoundRender_Emitter* E) = 0;
    virtual void render() = 0;
    virtual void rewind() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;

    virtual void alAuxInit(ALuint slot) = 0;

    virtual void fill_parameters(CSoundRender_Core* core) = 0;
};
