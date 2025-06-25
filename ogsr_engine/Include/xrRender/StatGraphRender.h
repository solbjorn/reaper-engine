#ifndef StatGraphRender_included
#define StatGraphRender_included

class CStatGraph;

class IStatGraphRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IStatGraphRender);

public:
    virtual ~IStatGraphRender() { ; }
    virtual void Copy(IStatGraphRender& _in) = 0;

    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;

    virtual void OnRender(CStatGraph& owner) = 0;
};

#endif //	StatGraphRender_included
