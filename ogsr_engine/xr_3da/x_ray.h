#pragma once

// refs
class CGameFont;
class ILoadingScreen;

// definition
class CApplication : public pureFrame, public IEventReceiver
{
    RTTI_DECLARE_TYPEINFO(CApplication, pureFrame, IEventReceiver);

public:
    // levels
    struct sLevelInfo
    {
        char* folder;
    };

    ILoadingScreen* loadingScreen;

    u32 ll_dwReference;

    EVENT eQuit;
    EVENT eStart;
    EVENT eStartLoad;
    EVENT eDisconnect;

    void Level_Append(LPCSTR lname);

    int load_stage;
    int max_load_stage;

    // Levels
    xr_vector<sLevelInfo> Levels;
    u32 Level_Current;
    void Level_Scan();
    [[nodiscard]] gsl::index Level_ID(gsl::czstring name) const;
    void Level_Set(u32 ID);

    // Loading
    void LoadBegin();
    void LoadEnd();
    void LoadTitleInt(); // 100 советов по выживанию в Зоне
    [[nodiscard]] tmc::task<void> LoadStage();
    [[nodiscard]] tmc::task<void> LoadDraw();
    void LoadForceFinish();

    void SetLoadStageTitle(pcstr ls_title);

    [[nodiscard]] tmc::task<void> OnEvent(EVENT E, u64 P1, u64 P2) override;

    // Other
    CApplication();
    ~CApplication() override;

    [[nodiscard]] tmc::task<void> OnFrame() override;
    void load_draw_internal();
    void SetLoadingScreen(ILoadingScreen* newScreen);
    void DestroyLoadingScreen();
};

extern bool IS_OGSR_GA;
extern BOOL g_appLoaded;
extern string512 g_sBenchmarkName;
extern CApplication* pApp;
