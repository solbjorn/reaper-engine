#ifndef ThunderboltDescRender_included
#define ThunderboltDescRender_included

class XR_NOVTABLE IThunderboltDescRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IThunderboltDescRender);

public:
    ~IThunderboltDescRender() override = 0;

    virtual void Copy(IThunderboltDescRender& _in) = 0;

    virtual void CreateModel(LPCSTR m_name) = 0;
    virtual void DestroyModel() = 0;
};

inline IThunderboltDescRender::~IThunderboltDescRender() = default;

#endif //	ThunderboltDescRender_included
