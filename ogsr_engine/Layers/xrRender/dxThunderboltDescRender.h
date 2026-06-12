#ifndef dxThunderboltDescRender_included
#define dxThunderboltDescRender_included

#include "..\..\Include\xrRender\ThunderboltDescRender.h"

class IRender_DetailModel;

class dxThunderboltDescRender : public IThunderboltDescRender
{
    RTTI_DECLARE_TYPEINFO(dxThunderboltDescRender, IThunderboltDescRender);

public:
    ~dxThunderboltDescRender() override = default;

    void Copy(IThunderboltDescRender& _in) override;

    void CreateModel(gsl::czstring m_name) override;
    void DestroyModel() override;

    IRender_DetailModel* l_model;
};

#endif //	dxThunderboltDescRender_included
