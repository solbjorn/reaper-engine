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

    virtual void Init(float x, float y, float width, float height);

    // CUIOptionsItem
    virtual void SetCurrentValue();
    virtual void SaveValue();
    virtual bool IsChanged();

    virtual void OnBtnUpClick();
    virtual void OnBtnDownClick();

    void SetMax(int max) { m_iMax = max; }
    void SetMin(int min) { m_iMin = min; }
    int Value() const { return m_iVal; }

protected:
    void SetValue();
    virtual bool CanPressUp();
    virtual bool CanPressDown();
    virtual void IncVal();
    virtual void DecVal();

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

    virtual void Init(float x, float y, float width, float height);

    // CUIOptionsItem
    virtual void SetCurrentValue();
    virtual void SaveValue();
    virtual bool IsChanged();

    virtual void OnBtnUpClick();
    virtual void OnBtnDownClick();

    void SetMax(float max);
    void SetMin(float min);

protected:
    void SetValue();
    virtual bool CanPressUp();
    virtual bool CanPressDown();
    virtual void IncVal();
    virtual void DecVal();

    float m_fMax{100.0f};
    float m_fMin{};
    float m_fStep{0.1f};
    float m_fVal{};
};
XR_SOL_BASE_CLASSES(CUISpinFlt);

#endif // __XR_UISPINNUM_H
