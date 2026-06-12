// file:		UISpinNum.h
// description:	Spin Button with numerical data (unlike text data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#ifndef __XR_UISPINNUM_H
#define __XR_UISPINNUM_H

#include "UICustomSpin.h"

class CUISpinNum : public CUICustomSpin
{
    RTTI_DECLARE_TYPEINFO(CUISpinNum, CUICustomSpin);

public:
    CUISpinNum();
    ~CUISpinNum() override = default;

    void Init(f32 x, f32 y, f32 width, f32 height) override;

    // CUIOptionsItem
    void SetCurrentValue() override;
    void SaveValue() override;
    [[nodiscard]] bool IsChanged() override;

    void OnBtnUpClick() override;
    void OnBtnDownClick() override;

    void SetMax(s32 max) { m_iMax = max; }
    void SetMin(s32 min) { m_iMin = min; }
    [[nodiscard]] s32 Value() const { return m_iVal; }

protected:
    void SetValue();
    [[nodiscard]] bool CanPressUp() override;
    [[nodiscard]] bool CanPressDown() override;
    void IncVal() override;
    void DecVal() override;

    int m_iMax{100};
    int m_iMin{};
    int m_iStep{1};
    int m_iVal{};
};
XR_SOL_BASE_CLASSES(CUISpinNum);

class CUISpinFlt : public CUICustomSpin
{
    RTTI_DECLARE_TYPEINFO(CUISpinFlt, CUICustomSpin);

public:
    CUISpinFlt();
    ~CUISpinFlt() override = default;

    void Init(f32 x, f32 y, f32 width, f32 height) override;

    // CUIOptionsItem
    void SetCurrentValue() override;
    void SaveValue() override;
    [[nodiscard]] bool IsChanged() override;

    void OnBtnUpClick() override;
    void OnBtnDownClick() override;

    void SetMax(f32 max);
    void SetMin(f32 min);

protected:
    void SetValue();
    [[nodiscard]] bool CanPressUp() override;
    [[nodiscard]] bool CanPressDown() override;
    void IncVal() override;
    void DecVal() override;

    float m_fMax{100.0f};
    float m_fMin{};
    float m_fStep{0.1f};
    float m_fVal{};
};
XR_SOL_BASE_CLASSES(CUISpinFlt);

#endif // __XR_UISPINNUM_H
