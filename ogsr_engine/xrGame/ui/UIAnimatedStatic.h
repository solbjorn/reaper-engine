#ifndef UI_ANIMATED_STATIC_H_
#define UI_ANIMATED_STATIC_H_

#include "UIStatic.h"

class CUIAnimatedStatic : public CUIStatic
{
    RTTI_DECLARE_TYPEINFO(CUIAnimatedStatic, CUIStatic);

private:
    typedef CUIStatic inherited;

    // Количекство кадров анимации
    u32 m_uFrameCount{};
    // Текущий фрейм
    u32 m_uCurFrame{std::numeric_limits<u32>::max()};
    // Размеры текстуры с анимацией в кадрах
    u32 m_uAnimCols{std::numeric_limits<u32>::max()};
    // Размеры кадра на тектуре
    u32 m_uFrameWidth{};
    u32 m_uFrameHeight{};
    // Время показа всей анимации в ms.
    u32 m_uAnimationDuration{};
    // Время прошедшее с начала анимации
    u32 m_uTimeElapsed{};
    // флаг-признак необходимости пересчета статичных параметров анимации
    bool m_bParamsChanged{true};
    // Признак проигрывания анимации
    bool m_bPlaying{};

public:
    // Флаг-признак циклического проигрывания
    bool m_bCyclic{true};

private:
    u32 m_prevTime{};
    Fvector2 m_pos{};

    // Инициализация первого кадра
    // Params:	frameNum	- номер кадра: [0..m_uFrameCount)
    void SetFrame(const u32 frameNum);

public:
    CUIAnimatedStatic();
    ~CUIAnimatedStatic() override = default;

    // Устанавливаем параметры
    void SetOffset(float x, float y) { m_pos.set(x, y); }
    void SetFramesCount(u32 frameCnt)
    {
        m_uFrameCount = frameCnt;
        m_bParamsChanged = true;
    }
    void SetAnimCols(u32 animCols)
    {
        m_uAnimCols = animCols;
        m_bParamsChanged = true;
    }
    void SetAnimationDuration(u32 animDur)
    {
        m_uAnimationDuration = animDur;
        m_bParamsChanged = true;
    }
    void SetFrameDimentions(u32 frameW, u32 frameH)
    {
        m_uFrameHeight = frameH;
        m_uFrameWidth = frameW;
        m_bParamsChanged = true;
    }
    // Управление
    void Play()
    {
        m_bPlaying = true;
        m_prevTime = Device.dwTimeContinual;
    }
    void Stop() { m_bPlaying = false; }
    void Rewind(u32 delta = 0)
    {
        m_uCurFrame = 0xffffffff;
        m_uTimeElapsed = delta;
    }
    void SetAnimPos(float pos);

    virtual void Update();
};

#endif // UI_ANIMATED_STATIC_H_
