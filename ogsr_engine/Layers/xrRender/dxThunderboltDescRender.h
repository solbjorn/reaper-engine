#ifndef dxThunderboltDescRender_included
#define dxThunderboltDescRender_included

#include "..\..\Include\xrRender\ThunderboltDescRender.h"

class IRender_DetailModel;

class dxThunderboltDescRender : public IThunderboltDescRender
{
    RTTI_DECLARE_TYPEINFO(dxThunderboltDescRender, IThunderboltDescRender);

public:
    virtual void Copy(IThunderboltDescRender& _in);

    virtual void CreateModel(LPCSTR m_name);
    virtual void DestroyModel();
    // private:
public:
    IRender_DetailModel* l_model;
};

#endif //	dxThunderboltDescRender_included
