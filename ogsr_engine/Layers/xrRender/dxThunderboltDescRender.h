#ifndef dxThunderboltDescRender_included
#define dxThunderboltDescRender_included

#include "..\..\Include\xrRender\ThunderboltDescRender.h"

class IRender_DetailModel;

class dxThunderboltDescRender : public IThunderboltDescRender
{
    RTTI_DECLARE_TYPEINFO(dxThunderboltDescRender, IThunderboltDescRender);

public:
    ~dxThunderboltDescRender() override = default;

    virtual void Copy(IThunderboltDescRender& _in);

    virtual void CreateModel(LPCSTR m_name);
    virtual void DestroyModel();

    IRender_DetailModel* l_model;
};

#endif //	dxThunderboltDescRender_included
