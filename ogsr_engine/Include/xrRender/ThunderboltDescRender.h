#ifndef ThunderboltDescRender_included
#define ThunderboltDescRender_included

class IThunderboltDescRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IThunderboltDescRender);

public:
    virtual ~IThunderboltDescRender() { ; }
    virtual void Copy(IThunderboltDescRender& _in) = 0;

    virtual void CreateModel(LPCSTR m_name) = 0;
    virtual void DestroyModel() = 0;
};

#endif //	ThunderboltDescRender_included
