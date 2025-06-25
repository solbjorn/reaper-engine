#ifndef ThunderboltRender_included
#define ThunderboltRender_included

class CEffect_Thunderbolt;

class IThunderboltRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IThunderboltRender);

public:
    virtual ~IThunderboltRender() { ; }
    virtual void Copy(IThunderboltRender& _in) = 0;

    virtual void Render(CEffect_Thunderbolt& owner) = 0;
};

#endif //	ThunderboltRender_included
