// file:		UISpinNum.h
// description:	Spin Button with text data (unlike numerical data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "UICustomSpin.h"

class CUISpinText : public CUICustomSpin
{
    RTTI_DECLARE_TYPEINFO(CUISpinText, CUICustomSpin);

public:
    CUISpinText();
    ~CUISpinText() override = default;

    // CUIOptionsItem
    void SetCurrentValue() override;
    void SaveValue() override;
    [[nodiscard]] bool IsChanged() override;

    // own
    void OnBtnUpClick() override;
    void OnBtnDownClick() override;

    void AddItem_(const char* item, int id);
    [[nodiscard]] gsl::czstring GetTokenText();

protected:
    [[nodiscard]] bool CanPressUp() override;
    [[nodiscard]] bool CanPressDown() override;
    void IncVal() override {}
    void DecVal() override {}
    void SetItem();

    struct SInfo
    {
        shared_str _orig;
        shared_str _transl;
        int _id;
    };
    typedef xr_vector<SInfo> Items;
    typedef Items::iterator Items_it;

    Items m_list;
    int m_curItem;
};
XR_SOL_BASE_CLASSES(CUISpinText);
