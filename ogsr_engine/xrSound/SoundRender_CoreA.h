#ifndef SoundRender_CoreAH
#define SoundRender_CoreAH

#include "SoundRender_Core.h"
#include "OpenALDeviceList.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wmissing-variable-declarations");

#include <eax.h>

XR_DIAG_POP();

#ifdef DEBUG

#define A_CHK(expr) \
    { \
        alGetError(); \
        expr; \
        ALenum error = alGetError(); \
        VERIFY2(error == AL_NO_ERROR, (LPCSTR)alGetString(error)); \
    } \
    XR_MACRO_END()

#define AC_CHK(expr) \
    { \
        alcGetError(pDevice); \
        expr; \
        ALCenum error = alcGetError(pDevice); \
        VERIFY2(error == ALC_NO_ERROR, (LPCSTR)alcGetString(pDevice, error)); \
    } \
    XR_MACRO_END()

#else

#define A_CHK(expr) \
    { \
        expr; \
    } \
    XR_MACRO_END()

#define AC_CHK(expr) \
    { \
        expr; \
    } \
    XR_MACRO_END()

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
    void i_eax_set(const GUID* guid, u32 prop, void* val, u32 sz) override;
    void i_eax_get(const GUID* guid, u32 prop, void* val, u32 sz) override;
    void update_listener(const Fvector3& P, const Fvector3& D, const Fvector3& N, const Fvector3& R, f32 dt) override;

    bool init_device_list();
    void init_device_properties(const bool& is_al_soft);

    bool init_context(const ALDeviceDesc& deviceDesc);
    void release_context();

    bool reopen_device(const char* deviceName) const;

public:
    CSoundRender_CoreA();
    ~CSoundRender_CoreA() override;

    void _initialize(s32 stage) override;
    void _clear() override;
    void _restart() override;

    void set_master_volume(f32 f) override;
};

#endif
