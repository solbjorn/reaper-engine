#ifndef ThunderboltRender_included
#define ThunderboltRender_included

class CEffect_Thunderbolt;

class XR_NOVTABLE IThunderboltRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IThunderboltRender);

public:
    virtual ~IThunderboltRender() = 0;

    virtual void Copy(IThunderboltRender& _in) = 0;

    virtual void Render(CEffect_Thunderbolt& owner) = 0;
};

inline IThunderboltRender::~IThunderboltRender() = default;

#endif //	ThunderboltRender_included
