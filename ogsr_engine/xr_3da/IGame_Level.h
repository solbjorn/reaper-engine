#ifndef igame_level_h_defined
#define igame_level_h_defined

#include "../xrCDB/xr_area.h"
#include "../xrSound/Sound.h"
#include "EventAPI.h"
#include "iinputreceiver.h"
// #include "CameraManager.h"
#include "xr_object_list.h"

// refs
class CCameraManager;
class CCursor;
class CCustomHUD;
class ISpatial;
namespace Feel
{
class Sound;
}

class CServerInfo
{
private:
    struct SItem_ServerInfo
    {
        string128 name;
        u32 color;
    };
    enum
    {
        max_item = 15
    };
    svector<SItem_ServerInfo, max_item> data;

public:
    u32 Size() { return data.size(); }
    void ResetData() { data.clear(); }

    void AddItem(LPCSTR name_, LPCSTR value_, u32 color_ = RGB(255, 255, 255));
    void AddItem(shared_str& name_, LPCSTR value_, u32 color_ = RGB(255, 255, 255));

    IC SItem_ServerInfo& operator[](u32 id)
    {
        VERIFY(id < max_item);
        return data[id];
    }

    // CServerInfo() {};
    //~CServerInfo() {};
};

//-----------------------------------------------------------------------------------------------------------
class XR_NOVTABLE IGame_Level : public DLL_Pure, public IInputReceiver, public pureRender, public pureFrame, public IEventReceiver
{
    RTTI_DECLARE_TYPEINFO(IGame_Level, DLL_Pure, IInputReceiver, pureRender, pureFrame, IEventReceiver);

protected:
    // Network interface
    CObject* pCurrentEntity;
    CObject* pCurrentViewEntity;

    // Static sounds
    xr_vector<ref_sound> Sounds_Random;
    u32 Sounds_Random_dwNextTime{};
    BOOL Sounds_Random_Enabled{};
    CCameraManager* m_pCameras;
    xr_string on_change_weather_callback;

    // temporary
    xr_vector<ISpatial*> snd_ER;

public:
    CObjectList Objects;
    CObjectSpace ObjectSpace;

    CCameraManager& Cameras() const { return *m_pCameras; }

    BOOL bReady;

    CInifile* pLevel;
    CCustomHUD* pHUD;

public: // deferred sound events
    struct _esound_delegate
    {
        Feel::Sound* dest;
        ref_sound_data_ptr source;
        float power;
        float time_to_stop;
    };
    xr_vector<_esound_delegate> snd_Events;

public:
    // Main, global functions
    IGame_Level();
    virtual ~IGame_Level();

    virtual shared_str name() const = 0;

    virtual BOOL net_Start(LPCSTR op_server, LPCSTR op_client) = 0;
    virtual void net_Load(LPCSTR name) = 0;
    virtual void net_Save(LPCSTR name) = 0;
    virtual void net_Stop();
    virtual void net_Update() = 0;

    virtual BOOL Load(u32 dwNum);
    virtual BOOL Load_GameSpecific_Before() { return TRUE; }; // before object loading
    virtual BOOL Load_GameSpecific_After() { return TRUE; }; // after object loading

    virtual void Load_GameSpecific_CFORM_Serialize(IWriter& writer) = 0;
    virtual bool Load_GameSpecific_CFORM_Deserialize(IReader& reader) = 0;
    virtual void Load_GameSpecific_CFORM(CDB::TRI* T, size_t count) = 0;

    virtual void OnFrame(void);
    virtual void OnRender(void);

    // Main interface
    CObject* CurrentEntity(void) const { return pCurrentEntity; }
    CObject* CurrentViewEntity(void) const { return pCurrentViewEntity; }
    void SetEntity(CObject* O) { pCurrentEntity = pCurrentViewEntity = O; }
    void SetViewEntity(CObject* O) { pCurrentViewEntity = O; }

    void SoundEvent_Register(ref_sound_data_ptr S, float range, float time_to_stop);
    void SoundEvent_Dispatch();
    void SoundEvent_OnDestDestroy(Feel::Sound*);

    // Loader interface
    void LL_CheckTextures();
    virtual void SetEnvironmentGameTimeFactor(u64 const& GameTime, float const& fTimeFactor) = 0;
    virtual void OnChangeCurrentWeather(const char* sect) = 0;

    virtual void OnDestroyObject(u16 id) = 0;

    virtual void GetGameTimeForShaders(u32& hours, u32& minutes, u32& seconds, u32& milliseconds) = 0;
};

//-----------------------------------------------------------------------------------------------------------
extern IGame_Level* g_pGameLevel;

#endif
