#pragma once

#include "iinputreceiver.h"
#include "effector.h"
#include "GameFont.h"

class CDemoRecord : public CEffectorCam, public IInputReceiver, public pureRender
{
    RTTI_DECLARE_TYPEINFO(CDemoRecord, CEffectorCam, IInputReceiver, pureRender);

public:
    int iCount;
    IWriter* file{};

    Fmatrix m_Camera;
    u32 m_Stage;

    Fvector m_vT;
    Fvector m_vR;
    Fvector m_vVelocity;
    Fvector m_vAngularVelocity;

    bool m_bMakeCubeMap;
    bool m_bMakeScreenshot;
    int m_iLMScreenshotFragment;
    bool m_bMakeLevelMap;

    float m_fSpeed0;
    float m_fSpeed1;
    float m_fSpeed2;
    float m_fSpeed3;
    float m_fAngSpeed0;
    float m_fAngSpeed1;
    float m_fAngSpeed2;
    float m_fAngSpeed3;

    std::unique_ptr<CGameFont> pFontSystem;

    explicit CDemoRecord(const char* name, float life_time = 60.0f * 60.0f * 1000.0f);
    ~CDemoRecord() override;

    void MakeCubeMapFace(Fvector& D, Fvector& N);
    void MakeLevelMapProcess();
    void MakeScreenshotFace();
    void RecordKey();
    void MakeCubemap();
    void MakeScreenshot();
    void MakeLevelMapScreenshot(bool bHQ);

    void IR_OnKeyboardPress(int dik) override;
    void IR_OnKeyboardHold(int dik) override;
    void IR_OnKeyboardRelease(int dik) override;

    void IR_OnMousePress(int btn) override;
    void IR_OnMouseRelease(int btn) override;

    void IR_OnMouseMove(int dx, int dy) override;
    void IR_OnMouseHold(int btn) override;

    BOOL ProcessCam(SCamEffectorInfo& info) override;

    void OnRender() override
    {
        if (pFontSystem)
            pFontSystem->OnRender();
    }

    Fvector m_HPB;
    Fvector m_Position;
    bool m_b_redirect_input_to_level;
};
