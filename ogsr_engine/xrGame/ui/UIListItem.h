#pragma once

#include "UIbutton.h"

class CUIListItem : public CUIButton
{
    RTTI_DECLARE_TYPEINFO(CUIListItem, CUIButton);

private:
    friend class CUIListWnd;

    typedef CUIButton inherited;

public:
    CUIListItem();
    ~CUIListItem() override;

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void Init(gsl::czstring str, f32 x, f32 y, f32 width, f32 height) override;
    void InitTexture(gsl::czstring tex_name) override;

    void* GetData() { return m_pData; }
    void SetData(void* pData) { m_pData = pData; }

    int GetIndex() { return m_iIndex; }
    void SetIndex(int index)
    {
        m_iIndex = index;
        m_iGroupID = index;
    }

    int GetValue() { return m_iValue; }
    void SetValue(int value) { m_iValue = value; }

    int GetGroupID() { return m_iGroupID; }
    void SetGroupID(int ID) { m_iGroupID = ID; }

    virtual void MarkSelected(bool) {}
    // переопределяем критерий подсвечивания текста
    [[nodiscard]] bool IsHighlightText() override;
    virtual void SetHighlightText(bool Highlight) { m_bHighlightText = Highlight; }

protected:
    // указатель на произвольные данные, которые могут
    // присоедениены к элементу
    void* m_pData{};

    // произвольное число, приписанное объекту
    int m_iValue{};

    // индекс в списке
    int m_iIndex{-1};

    // идентификатор группы
    int m_iGroupID{-1};

    // подсвечивается кнопка или нет?
    bool m_bHighlightText{};
};
XR_SOL_BASE_CLASSES(CUIListItem);
