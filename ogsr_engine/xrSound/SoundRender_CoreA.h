#ifndef SoundRender_CoreAH
#define SoundRender_CoreAH

#include "SoundRender_Core.h"
#include "OpenALDeviceList.h"

#include <eax.h>

#ifdef DEBUG
#define A_CHK(expr) \
    { \
        alGetError(); \
        expr; \
        ALenum error = alGetError(); \
        VERIFY2(error == AL_NO_ERROR, (LPCSTR)alGetString(error)); \
    }
#define AC_CHK(expr) \
    { \
        alcGetError(pDevice); \
        expr; \
        ALCenum error = alcGetError(pDevice); \
        VERIFY2(error == ALC_NO_ERROR, (LPCSTR)alcGetString(pDevice, error)); \
    }
#else
#define A_CHK(expr) \
    { \
        expr; \
    }
#define AC_CHK(expr) \
    { \
        expr; \
    }
#endif

class CSoundRender_CoreA : public CSoundRender_Core
{
    RTTI_DECLARE_TYPEINFO(CSoundRender_CoreA, CSoundRender_Core);

public:
    typedef CSoundRender_Core inherited;

    EAXSet eaxSet; // EAXSet function, retrieved if EAX Extension is supported
    EAXGet eaxGet; // EAXGet function, retrieved if EAX Extension is supported

    ALCdevice* pDevice;
    ALCcontext* pContext;

    ALDeviceList* pDeviceList;

    struct SListenerSmooth
    {
        Fvector prevVelocity;
        Fvector curVelocity;
        Fvector accVelocity;
    };
    SListenerSmooth ListenerSmooth{};

    BOOL EAXQuerySupport(BOOL bDeferred, const GUID* guid, u32 prop, void* val, u32 sz);
    BOOL EAXTestSupport(BOOL bDeferred);

protected:
    virtual void i_eax_set(const GUID* guid, u32 prop, void* val, u32 sz);
    virtual void i_eax_get(const GUID* guid, u32 prop, void* val, u32 sz);
    virtual void update_listener(const Fvector& P, const Fvector& D, const Fvector& N, const Fvector& R, float dt);

    bool init_device_list();
    void init_device_properties(const bool& is_al_soft);

    bool init_context(const ALDeviceDesc& deviceDesc);
    void release_context();

    bool reopen_device(const char* deviceName) const;

public:
    CSoundRender_CoreA();
    virtual ~CSoundRender_CoreA();

    virtual void _initialize(int stage);
    virtual void _clear();
    virtual void _restart();

    virtual void set_master_volume(float f);
};

#endif
